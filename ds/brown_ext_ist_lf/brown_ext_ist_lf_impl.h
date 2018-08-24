#ifndef BROWN_EXT_IST_LF_IMPL_H
#define BROWN_EXT_IST_LF_IMPL_H

#include "brown_ext_ist_lf.h"

template <typename K, typename V, class Interpolate, class RecManager>
V istree<K,V,Interpolate,RecManager>::find(const int tid, const K& key) {
    auto guard = recordmgr->getGuard(tid, true);
    casword_t ptr = prov->readPtr(tid, root->ptrAddr(0));
    assert(ptr);
    Node<K,V> * parent = root;
    int ixToPtr = 0;
    while (true) {
        if (unlikely(IS_KVPAIR(ptr))) {
            auto kv = CASWORD_TO_KVPAIR(ptr);
            if (kv->k == key) return kv->v;
            return NO_VALUE;
        } else if (unlikely(IS_REBUILDOP(ptr))) {
            auto rebuild = CASWORD_TO_REBUILDOP(ptr);
            ptr = NODE_TO_CASWORD(rebuild->candidate);
        } else if (IS_NODE(ptr)) {
            // ptr is an internal node
            parent = CASWORD_TO_NODE(ptr);
            assert(parent);
            ixToPtr = interpolationSearch(tid, key, parent);
            ptr = prov->readPtr(tid, parent->ptrAddr(ixToPtr));
        } else {
            assert(IS_VAL(ptr));
            assert(CASWORD_IS_EMPTY_VAL(ptr) || ixToPtr > 0); // invariant: leftmost pointer cannot contain a non-empty VAL (it contains a non-NULL pointer or an empty val casword)
            if (CASWORD_IS_EMPTY_VAL(ptr)) return NO_VALUE;
            auto v = CASWORD_TO_VAL(ptr);
            auto ixToKey = ixToPtr - 1;
            return (parent->key(ixToKey) == key) ? v : NO_VALUE;
        }
    }
}

template <typename K, typename V, class Interpolate, class RecManager>
size_t istree<K,V,Interpolate,RecManager>::markAndCount(const int tid, const casword_t ptr) {
    assert(!recordmgr->isQuiescent(tid));
    if (unlikely(IS_KVPAIR(ptr))) {
        return 1;
    } else if (unlikely(IS_VAL(ptr))) {
        return 1 - CASWORD_IS_EMPTY_VAL(ptr);
    } else if (unlikely(IS_REBUILDOP(ptr))) {
        auto op = CASWORD_TO_REBUILDOP(ptr);
        return markAndCount(tid, NODE_TO_CASWORD(op->candidate));
        // if we are here seeing this rebuildop,
        // then we ALREADY marked the node that points to the rebuildop,
        // which means that rebuild op cannot possible change that node
        // to effect the rebuilding.
    }
    
    assert(IS_NODE(ptr));
    auto node = CASWORD_TO_NODE(ptr);
    
    // optimize by taking the sum from node->dirty if we run into a finished subtree
    auto result = node->dirty;
    if (result & DIRTY_FINISHED) return DIRTY_FINISHED_TO_SUM(result); // markAndCount has already FINISHED in this subtree, and sum is the count

    if ((result & DIRTY_STARTED) == 0) {
        result = __sync_val_compare_and_swap(&node->dirty, 0, DIRTY_STARTED);
        if (result & DIRTY_FINISHED) return DIRTY_FINISHED_TO_SUM(result); // markAndCount has already FINISHED in this subtree, and sum is the count
    }
    
    // optimize for contention by first claiming a subtree to recurse on
    // THEN after there are no more subtrees to claim, help (any that are still DIRTY_STARTED)
    size_t keyCount = 0;
    while (1) {
        auto ix = __sync_fetch_and_add(&node->nextMarkAndCount, 1);
        if (ix >= node->degree) break;
        keyCount += markAndCount(tid, prov->readPtr(tid, node->ptrAddr(ix)));
    }
    
    // recurse over all subtrees
    for (int i=0;i<node->degree;++i) {
        keyCount += markAndCount(tid, prov->readPtr(tid, node->ptrAddr(i)));
    }
    
    if (keyCount) __sync_bool_compare_and_swap(&node->dirty, DIRTY_STARTED, SUM_TO_DIRTY_FINISHED(keyCount));
    return keyCount;
}

template <typename K, typename V, class Interpolate, class RecManager>
void istree<K,V,Interpolate,RecManager>::createIdeal(const int tid, casword_t ptr, IdealBuilder * b) {
    assert(!recordmgr->isQuiescent(tid));
    if (unlikely(IS_KVPAIR(ptr))) {
        auto pair = CASWORD_TO_KVPAIR(ptr);
        b->addKV(tid, pair->k, pair->v);
    } else if (unlikely(IS_REBUILDOP(ptr))) {
        auto op = CASWORD_TO_REBUILDOP(ptr);
        createIdeal(tid, NODE_TO_CASWORD(op->candidate), b);
    } else {
        //if (!IS_NODE(ptr)) printf("ptr=%lld is not a node! %s\n", ptr, (CASWORD_IS_EMPTY_VAL(ptr)) ? "is empty val\n" : IS_VAL(ptr) ? "is val" : "not sure...");
        assert(IS_NODE(ptr));
        auto node = CASWORD_TO_NODE(ptr);
        for (int i=0;i<node->degree;++i) {
            auto childptr = prov->readPtr(tid, node->ptrAddr(i));
            if (IS_VAL(childptr)) {
                if (CASWORD_IS_EMPTY_VAL(childptr)) continue;
                auto v = CASWORD_TO_VAL(childptr);
                assert(i > 0);
                auto k = node->key(i - 1); // it's okay that this read is not atomic with the value read, since keys of nodes do not change. (so, we can linearize the two reads when we read the value.)
                b->addKV(tid, k, v);
            } else {
                createIdeal(tid, childptr, b);
            }
        }
    }
}

template <typename K, typename V, class Interpolate, class RecManager>
casword_t istree<K,V,Interpolate,RecManager>::createIdeal(const int tid, RebuildOperation<K,V> * op, const size_t keyCount) {
    assert(!recordmgr->isQuiescent(tid));
    if (keyCount == 0) return EMPTY_VAL_TO_CASWORD;
    IdealBuilder b (this, keyCount, op->depth);
    createIdeal(tid, NODE_TO_CASWORD(op->candidate), &b);
    return b.getCASWord(tid);
}

template <typename K, typename V, class Interpolate, class RecManager>
void istree<K,V,Interpolate,RecManager>::helpRebuild(const int tid, RebuildOperation<K,V> * op) {
#ifdef MEASURE_REBUILDING_TIME
    GSTATS_TIMER_RESET(tid, timer_rebuild);
#endif
    assert(!recordmgr->isQuiescent(tid));
    auto keyCount = markAndCount(tid, NODE_TO_CASWORD(op->candidate));
    auto oldWord = REBUILDOP_TO_CASWORD(op);
    casword_t newWord = createIdeal(tid, op, keyCount);
    if (prov->dcssPtr(tid, (casword_t *) &op->parent->dirty, 0, (casword_t *) op->parent->ptrAddr(op->index), oldWord, newWord).status == DCSS_SUCCESS) {
        GSTATS_ADD_IX(tid, num_complete_rebuild_at_depth, 1, op->depth);
        freeSubtree(tid, op->candidate, true);
        recordmgr->retire(tid, op);
    } else {
        if (unlikely(IS_KVPAIR(newWord))) {
            recordmgr->deallocate(tid, CASWORD_TO_KVPAIR(newWord));
        } else if (unlikely(IS_VAL(newWord))) {
            
        } else {
            assert(IS_NODE(newWord));
            freeSubtree(tid, CASWORD_TO_NODE(newWord), false);
        }
    }
#ifdef MEASURE_REBUILDING_TIME
    auto cappedDepth = std::min((size_t) 9, op->depth);
    GSTATS_ADD_IX(tid, elapsed_rebuild_depth, GSTATS_TIMER_ELAPSED(tid, timer_rebuild), cappedDepth);
#endif
}

template <typename K, typename V, class Interpolate, class RecManager>
void istree<K,V,Interpolate,RecManager>::rebuild(const int tid, Node<K,V> * rebuildRoot, Node<K,V> * parent, int index /* of rebuildRoot in parent */, const size_t depth) {
    assert(!recordmgr->isQuiescent(tid));
    auto op = new RebuildOperation<K,V>(rebuildRoot, parent, index, depth);
    auto ptr = REBUILDOP_TO_CASWORD(op);
    auto old = NODE_TO_CASWORD(op->candidate);
    assert(op->parent == parent);
    if (prov->dcssPtr(tid, (casword_t *) &op->parent->dirty, 0, (casword_t *) op->parent->ptrAddr(op->index), old, ptr).status == DCSS_SUCCESS) {
        helpRebuild(tid, op);
    } else {
        recordmgr->deallocate(tid, op);
    }
}

template <typename K, typename V, class Interpolate, class RecManager>
int istree<K,V,Interpolate,RecManager>::interpolationSearch(const int tid, const K& key, Node<K,V> * const node) {
    assert(!recordmgr->isQuiescent(tid));
    // TODO: redo prefetching, taking into account the fact that l2 adjacent line prefetcher DOESN'T grab an adjacent line
    __builtin_prefetch(&node->degree, 1);
    __builtin_prefetch(&node->maxKey, 1);
    __builtin_prefetch((node->keyAddr(0)), 1);
    __builtin_prefetch((node->keyAddr(0))+(8), 1);
    __builtin_prefetch((node->keyAddr(0))+(16), 1);

    //assert(node->degree >= 1);
    if (unlikely(node->degree == 1)) {
//        GSTATS_APPEND_D(tid, visited_in_isearch, 1);
        return 0;
    }
    
    const int numKeys = node->degree - 1;
    const K& minKey = node->minKey;
    const K& maxKey = node->maxKey;
    
    if (unlikely(key < minKey)) {
//        GSTATS_APPEND_D(tid, visited_in_isearch, 1);
        return 0;
    }
    if (unlikely(key >= maxKey)) {
//        GSTATS_APPEND_D(tid, visited_in_isearch, 1);
        return numKeys;
    }
    // assert: minKey <= key < maxKey
    int ix = (numKeys * (key - minKey) / (maxKey - minKey));

    __builtin_prefetch((node->keyAddr(0))+(ix-8), 1); // prefetch approximate key location
    __builtin_prefetch((node->keyAddr(0))+(ix), 1); // prefetch approximate key location
    __builtin_prefetch((node->keyAddr(0))+(ix+8), 1); // prefetch approximate key location
    __builtin_prefetch((node->keyAddr(0))+(numKeys+ix-8), 1); // prefetch approximate pointer location to accelerate later isDcss check
    __builtin_prefetch((node->keyAddr(0))+(numKeys+ix), 1); // prefetch approximate pointer location to accelerate later isDcss check
    __builtin_prefetch((node->keyAddr(0))+(numKeys+ix+8), 1); // prefetch approximate pointer location to accelerate later isDcss check
    
    const K& ixKey = node->key(ix);
//    std::cout<<"key="<<key<<" minKey="<<minKey<<" maxKey="<<maxKey<<" ix="<<ix<<" ixKey="<<ixKey<<std::endl;
    if (key < ixKey) {
        // search to the left for node.key[i] <= key, then return i+1
        int i;
        for (i=ix-1;i>=0;--i) {
            if (unlikely(key >= node->key(i))) {
//                GSTATS_APPEND_D(tid, visited_in_isearch, (ix-1) - i + 1);
                return i+1;
            }
        }
        assert(false);
    } else if (key > ixKey) {
        int i;
        for (i=ix+1;i<numKeys;++i) { // recall: degree - 1 keys vs degree pointers
            if (unlikely(key < node->key(i))) {
//                GSTATS_APPEND_D(tid, visited_in_isearch, i - (ix+1) + 1);
                return i;
            }
        }
        assert(false);
    } else {
//        GSTATS_APPEND_D(tid, visited_in_isearch, 1);
        return ix+1;
    }
}

// note: val is unused if t == Erase
template <typename K, typename V, class Interpolate, class RecManager>
V istree<K,V,Interpolate,RecManager>::doUpdate(const int tid, const K& key, const V& val, UpdateType t) {
    auto guard = recordmgr->getGuard(tid);

    const double SAMPLING_PROB = 1.;
    const int MAX_PATH_LENGTH = 64; // in practice, the depth is probably less than 10 even for many billions of keys. max is technically nthreads + O(log log n), but this requires an astronomically unlikely event.
    Node<K,V> * path[MAX_PATH_LENGTH]; // stack to save the path
    int pathLength;
    Node<K,V> * node;

retry:
    pathLength = 0;
    node = root;
    while (true) {
        auto ix = interpolationSearch(tid, key, node);
retryNode:
        bool affectsChangeSum = true;
        auto word = prov->readPtr(tid, node->ptrAddr(ix));
        if (IS_KVPAIR(word) || IS_VAL(word)) {
            KVPair<K,V> * pair = NULL;
            Node<K,V> * newNode = NULL;
            KVPair<K,V> * newPair = NULL;
            auto newWord = (casword_t) NULL;

            assert(CASWORD_IS_EMPTY_VAL(word) || !IS_VAL(word) || ix > 0);
            auto foundKey = INF_KEY;
            auto foundVal = NO_VALUE;
            if (IS_VAL(word)) {
                foundKey = (CASWORD_IS_EMPTY_VAL(word)) ? INF_KEY : foundKey = node->key(ix - 1);
                if (!CASWORD_IS_EMPTY_VAL(word)) foundVal = CASWORD_TO_VAL(word);
            } else {
                assert(IS_KVPAIR(word));
                pair = CASWORD_TO_KVPAIR(word);
                foundKey = pair->k;
                foundVal = pair->v;
            }
            assert(foundVal == NO_VALUE || foundVal > 0 && foundKey < INF_KEY/8); // value must have top 3 bits empty so we can shift it
            
            if (foundKey == key) {
                if (t == InsertReplace) {
                    newWord = VAL_TO_CASWORD(val);
                    if (foundVal != NO_VALUE) affectsChangeSum = false; // note: should NOT count towards changeSum, because it cannot affect the complexity of operations
                } else if (t == InsertIfAbsent) {
                    if (foundVal != NO_VALUE) return foundVal;
                    newWord = VAL_TO_CASWORD(val);
                } else {
                    assert(t == Erase);
                    if (foundVal == NO_VALUE) return NO_VALUE;
                    newWord = EMPTY_VAL_TO_CASWORD;
                }
            } else {
                if (t == InsertReplace || t == InsertIfAbsent) {
                    if (foundVal == NO_VALUE) {
                        // after the insert, this pointer will lead to only one kvpair in the tree,
                        // so we just create a kvpair instead of a node
                        newPair = createKVPair(tid, key, val);
                        newWord = KVPAIR_TO_CASWORD(newPair);
                    } else {
                        // there would be 2 kvpairs, so we create a node
                        KVPair<K,V> pairs[2];
                        if (key < foundKey) {
                            pairs[0] = { key, val };
                            pairs[1] = { foundKey, foundVal };
                        } else {
                            pairs[0] = { foundKey, foundVal };
                            pairs[1] = { key, val };
                        }
                        newNode = createLeaf(tid, pairs, 2);
                        newWord = NODE_TO_CASWORD(newNode);
                        foundVal = NO_VALUE; // the key we are inserting had no current value
                    }
                } else {
                    assert(t == Erase);
                    return NO_VALUE;
                }
            }
            assert(newWord);
            assert((newWord & (~TOTAL_MASK)));
            
            // DCSS that performs the update
            assert(ix >= 0);
            assert(ix < node->degree);
            auto result = prov->dcssPtr(tid, (casword_t *) &node->dirty, 0, (casword_t *) node->ptrAddr(ix), word, newWord);
            switch (result.status) {
                case DCSS_FAILED_ADDR2: // retry from same node
                    if (newPair) recordmgr->deallocate(tid, newPair);
                    if (newNode) freeNode(tid, newNode, false);
                    goto retryNode;
                    break;
                case DCSS_FAILED_ADDR1: // node is dirty; retry from root
                    if (newPair) recordmgr->deallocate(tid, newPair);
                    if (newNode) freeNode(tid, newNode, false);
                    goto retry;
                    break;
                case DCSS_SUCCESS:
                    assert(!recordmgr->isQuiescent(tid));
                    if (pair) recordmgr->retire(tid, pair);
                    
                    if (!affectsChangeSum) break;
                    
//                    // probabilistically increment the changeSums of ancestors
//                    if (myRNG->next() / (double) std::numeric_limits<uint64_t>::max() < SAMPLING_PROB) {
                        for (int i=0;i<pathLength;++i) {
                            path[i]->incrementChangeSum(tid, myRNG);
                        }
//                    }
                    
                    // now, we must determine whether we should rebuild
                    for (int i=0;i<pathLength;++i) {
//                        if ((path[i]->changeSum + (NUM_PROCESSES-1)) / (SAMPLING_PROB * (1 - EPS)) >= REBUILD_FRACTION * path[i]->initSize) {
                        if (path[i]->readChangeSum(tid, myRNG) >= REBUILD_FRACTION * path[i]->initSize) {
                            if (i == 0) {
#ifndef NO_REBUILDING
                                GSTATS_ADD_IX(tid, num_try_rebuild_at_depth, 1, 0);
                                rebuild(tid, path[0], root, 0, 0);
#endif
                            } else {
                                auto parent = path[i-1];
                                assert(parent->degree > 1);
                                assert(path[i]->degree > 1);
                                auto index = interpolationSearch(tid, path[i]->key(0), parent);

#ifndef NDEBUG
                                // single threaded only debug info
                                if (path[i]->degree == 1 || (TOTAL_THREADS == 1 && CASWORD_TO_NODE(parent->ptr(index)) != path[i])) {
                                    std::cout<<"i="<<i<<std::endl;
                                    std::cout<<"path length="<<pathLength<<std::endl;
                                    std::cout<<"parent@"<<(size_t) parent<<std::endl;
                                    std::cout<<"parent->degree="<<parent->degree<<std::endl;
                                    std::cout<<"parent keys";
                                    for (int j=0;j<parent->degree - 1;++j) std::cout<<" "<<parent->key(j);
                                    std::cout<<std::endl;
                                    std::cout<<"parent ptrs (converted)";
                                    for (int j=0;j<parent->degree;++j) std::cout<<" "<<(parent->ptr(j) & ~TOTAL_MASK);
                                    std::cout<<std::endl;
                                    std::cout<<"index="<<index<<std::endl;
                                    std::cout<<"parent->ptr(index) (converted)="<<(parent->ptr(index) & ~TOTAL_MASK)<<std::endl;
                                    std::cout<<"path[i]@"<<(size_t) path[i]<<std::endl;
                                    std::cout<<"path[i]->degree="<<path[i]->degree<<std::endl;
                                    std::cout<<"path[i]->key(0)="<<(path[i]->degree > 1 ? path[i]->key(0) : -1)<<std::endl;
                                    std::cout<<"path[i]->ptr(0)="<<(size_t) path[i]->ptr(0)<<std::endl;
                                    std::cout<<"path[i]->ptr(0) (converted)="<<(path[i]->ptr(0) & ~TOTAL_MASK)<<std::endl;
                                    std::cout<<"newWord="<<newWord<<std::endl;
                                    std::cout<<"newWord (converted)="<<(newWord & ~TOTAL_MASK)<<std::endl;
                                    if (IS_KVPAIR(newWord)) {
                                        std::cout<<"new key="<<CASWORD_TO_KVPAIR(newWord)->k<<std::endl;
                                    }
                                    std::cout<<"foundVal="<<foundVal<<std::endl;
                                    assert(false);
                                }
#endif
                                
#ifndef NO_REBUILDING
                                GSTATS_ADD_IX(tid, num_try_rebuild_at_depth, 1, i);
                                rebuild(tid, path[i], parent, index, i);
#endif
                            }
                            break;
                        }
                    }
                    break;
                default:
                    setbench_error("impossible switch case");
                    break;
            }
            return foundVal;
        } else if (IS_REBUILDOP(word)) {
            //std::cout<<"found supposed rebuildop "<<(size_t) word<<" at path length "<<pathLength<<std::endl;
            helpRebuild(tid, CASWORD_TO_REBUILDOP(word));
            goto retry;
        } else {
            assert(IS_NODE(word));
            node = CASWORD_TO_NODE(word);
            path[pathLength++] = node; // push on stack
            assert(pathLength <= MAX_PATH_LENGTH);
        }
    }
}

template <typename K, typename V, class Interpolate, class RecManager>
Node<K,V>* istree<K,V,Interpolate,RecManager>::createNode(const int tid, const int degree) {
    size_t sz = sizeof(Node<K,V>) + sizeof(K) * (degree - 1) + sizeof(casword_t) * degree;
    Node<K,V> * node = (Node<K,V> *) ::operator new (sz); //(Node<K,V> *) new char[sz];
//    std::cout<<"node of degree "<<degree<<" allocated size "<<sz<<" @ "<<(size_t) node<<std::endl;
    assert((((size_t) node) & TOTAL_MASK) == 0);
    node->capacity = degree;
    node->degree = 0;
    node->initSize = 0;
    node->changeSum = 0;
#ifdef IST_USE_MULTICOUNTER_AT_ROOT
    node->externalChangeCounter = NULL;
    assert(!node->externalChangeCounter);
#endif
    node->dirty = 0;
    node->nextMarkAndCount = 0;
    assert(node);
    return node;
}

template <typename K, typename V, class Interpolate, class RecManager>
Node<K,V>* istree<K,V,Interpolate,RecManager>::createLeaf(const int tid, KVPair<K,V> * pairs, int numPairs) {
    auto node = createNode(tid, numPairs+1);
    node->degree = numPairs+1;
    node->initSize = numPairs;
    *node->ptrAddr(0) = EMPTY_VAL_TO_CASWORD;
    for (int i=0;i<numPairs;++i) {
#ifndef NDEBUG
        if (i && pairs[i].k <= pairs[i-1].k) {
            std::cout<<"pairs";
            for (int j=0;j<numPairs;++j) {
                std::cout<<" "<<pairs[j].k;
            }
            std::cout<<std::endl;
        }
#endif
        assert(i==0 || pairs[i].k > pairs[i-1].k);
        node->key(i) = pairs[i].k;
        *node->ptrAddr(i+1) = VAL_TO_CASWORD(pairs[i].v);
    }
    node->minKey = node->key(0);
    node->maxKey = node->key(node->degree-2);
    return node;    
}

template <typename K, typename V, class Interpolate, class RecManager>
Node<K,V>* istree<K,V,Interpolate,RecManager>::createMultiCounterNode(const int tid, const int degree) {
//    GSTATS_ADD(tid, num_multi_counter_node_created, 1);
    auto node = createNode(tid, degree);
#ifdef IST_USE_MULTICOUNTER_AT_ROOT
    node->externalChangeCounter = new MultiCounter(this->NUM_PROCESSES, 1);
//    std::cout<<"created MultiCounter at address "<<node->externalChangeCounter<<std::endl;
    assert(node->externalChangeCounter);
#endif
    return node;
}

template <typename K, typename V, class Interpolate, class RecManager>
KVPair<K,V> * istree<K,V,Interpolate,RecManager>::createKVPair(const int tid, const K& key, const V& value) {
    auto result = new KVPair<K,V>(); //key, value);
    *result = { key, value };
//    std::cout<<"kvpair allocated of size "<<sizeof(KVPair<K,V>)<<" with key="<<key<<" @ "<<(size_t) result<<std::endl;
    assert((((size_t) result) & TOTAL_MASK) == 0);
    assert(result);
    return result;
}

#endif /* BROWN_EXT_IST_LF_IMPL_H */
