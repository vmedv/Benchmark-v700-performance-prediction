#ifndef BROWN_EXT_IST_LF_IMPL_H
#define BROWN_EXT_IST_LF_IMPL_H

#include "brown_ext_ist_lf.h"

// TODO: memory reclamation
// TODO: sequential rebuilding
// TODO: lock-free collaborative rebuilding
// TODO: remember to set initSize appropriately for nodes during rebuilding!

template <typename K, typename V, class Interpolate, class RecManager>
V istree<K,V,Interpolate,RecManager>::find(const int tid, const K& key) {
    casword_t ptr = prov->readPtr(tid, root->ptrAddr(0));
    assert(ptr);
    while (true) {
        if (unlikely(IS_KVPAIR(ptr))) {
            auto kv = CASWORD_TO_KVPAIR(ptr);
            if (kv->k == key) return kv->v;
            return NO_VALUE;
        } else if (unlikely(IS_REBUILDOP(ptr))) {
            auto rebuild = CASWORD_TO_REBUILDOP(ptr);
            ptr = NODE_TO_CASWORD(rebuild->candidate);
        } else {
            assert(IS_NODE(ptr));
            assert(ptr);
            // ptr is an internal node
            auto node = CASWORD_TO_NODE(ptr);
            const int ix = interpolationSearch(tid, key, node);
            ptr = prov->readPtr(tid, node->ptrAddr(ix));
        }
    }
}

//template <typename K, typename V, class Interpolate, class RecManager>
//size_t istree<K,V,Interpolate,RecManager>::dfsMark(const int tid, const casword_t ptr) {
//    if (IS_KVPAIR(ptr)) {
//        return 1;
//    }
//    if (IS_REBUILDOP(ptr)) {
//        auto op = CASWORD_TO_REBUILDOP(ptr);
//        return dfsMark(tid, NODE_TO_CASWORD(op->candidate));
//    }
//    auto node = CASWORD_TO_NODE(ptr);
//    
//    __sync_bool_compare_and_swap(&node->dirty, 0, 1);
//    
//    size_t sum = 0;
//    for (int i=0;i<node->degree;++i) {
//        sum += dfsMark(tid, prov->readPtr(tid, node->ptrAddr(i)));
//    }
//    
//    if (sum) __sync_bool_compare_and_swap(&node->dirty, 1, SUM_TO_DIRTY(sum));
//    return sum;
//}
//
//template <typename K, typename V, class Interpolate, class RecManager>
//Node<K,V> * istree<K,V,Interpolate,RecManager>::dfsBuild(const int tid, Node<K,V> ** const currAtDepth, const int buildDepth) {
//    return NULL;
//}
//
template <typename K, typename V, class Interpolate, class RecManager>
void istree<K,V,Interpolate,RecManager>::helpRebuild(const int tid, RebuildOperation<K,V> * op) {
//    // dfs to mark nodes and compute up to date subtree sizes
//    size_t size = dfsMark(tid, NODE_TO_CASWORD(op->candidate));
//    
//    /**
//     * rebuild subtree
//     */
//    
//    
//    /**
//     * TODO: redo with tuned probabilistic tree building
//     */
//    
//    // compute lookup table
//    const int MAX_DEPTH = 9; // repeated sqrt on 2^64 hits 1 after 7 iterations.
//    const int MIN_NODE_DEGREE = 4;
//    size_t nodeDegreeAtDepth[MAX_DEPTH];
//    nodeDegreeAtDepth[0] = (size_t) std::ceil(std::pow((double) size, 0.5));
//    int buildDepth = 1;
//    for (int i=1;i<MAX_DEPTH;++i) {
//        nodeDegreeAtDepth[i] = (size_t) std::ceil(std::pow((double) nodeDegreeAtDepth[i-1], 0.5));
//        if (nodeDegreeAtDepth[i] >= MIN_NODE_DEGREE) ++buildDepth;
//    }
//    
//    // fold any levels with degree <= 3 into the last level
//    assert(buildDepth < MAX_DEPTH);
//    if (nodeDegreeAtDepth[buildDepth] == 3) {
//        nodeDegreeAtDepth[buildDepth - 1] *= 6;
//    } else {
//        nodeDegreeAtDepth[buildDepth - 1] *= 2;
//    }
//    
//    // build tree left to right with array of current/"open" nodes and a dfs
//    Node<K,V> * currAtDepth[MAX_DEPTH];
//    for (int i=0;i<buildDepth;++i) {
//        currAtDepth[i] = createNode(tid, nodeDegreeAtDepth[i]);
//        if (i > 0) currAtDepth[i]->parent = currAtDepth[i-1];
//    }
//    auto node = dfsBuild(tid, currAtDepth, buildDepth);
//    
//    // replace subtree and update parent counter
//    
}
//
template <typename K, typename V, class Interpolate, class RecManager>
void istree<K,V,Interpolate,RecManager>::rebuild(const int tid, Node<K,V> * rebuildRoot, Node<K,V> * parent, int index /* of rebuildRoot in parent */) {
    // TODO: REMEMBER TO SET MINKEY AND MAXKEY OF NODES!!!!!

    GSTATS_ADD(tid, num_rebuild, 1);
//    RebuildOperation<K,V> * op = new RebuildOperation<K,V>();
//    op->candidate = result.candidate;
//    op->parent = op->candidate->parent;
//    op->index = interpolationSearch(tid, op->candidate->key(0), op->parent);
//    casword_t ptr = REBUILDOP_TO_CASWORD(op);
//    casword_t old = NODE_TO_CASWORD(op->candidate);
//    
//    dcssresult_t dcssResult = prov->dcssPtr(tid, &op->parent->dirty, 0, op->parent->ptrAddr(op->index), old, ptr);
//    switch (dcssResult.status) {
//        case DCSS_FAILED_ADDR2: case DCSS_FAILED_ADDR1: {
//            break; // we are done
//        }
//        case DCSS_SUCCESS: {
//            helpRebuild(tid, op);
//            break;
//        }
//        default: {
//            setbench_error("executed default switch case");
//            break;
//        }
//    }
}

template <typename K, typename V, class Interpolate, class RecManager>
int istree<K,V,Interpolate,RecManager>::interpolationSearch(const int tid, const K& key, Node<K,V> * const node) {
    // TODO: redo prefetching, taking into account the fact that l2 adjacent line prefetcher DOESN'T grab an adjacent line
    __builtin_prefetch(&node->degree, 1);
    __builtin_prefetch(&node->maxKey, 1);
    __builtin_prefetch((node->keyAddr(0)), 1);
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

    __builtin_prefetch((node->keyAddr(0))+(ix), 1); // prefetch approximate key location
    __builtin_prefetch((node->keyAddr(0))+(numKeys+ix), 1); // prefetch approximate pointer location to accelerate later isDcss check
    
    const K& ixKey = node->key(ix);
//    std::cout<<"key="<<key<<" minKey="<<minKey<<" maxKey="<<maxKey<<" ix="<<ix<<" ixKey="<<ixKey<<std::endl; // TODO: delete this debug print
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
    const double SAMPLING_PROB = 0.1;
    const int MAX_PATH_LENGTH = 64; // in practice, the depth is probably less than 10 even for many billions of keys. max is technically nthreads + O(log log n), but this requires an astronomically unlikely event.
    Node<K,V> * path[MAX_PATH_LENGTH]; // stack to save the path
    int pathLength;
    Node<K,V> * node;

retry:
    pathLength = 0;
    node = root;
    while (true) {
retryNode:
        auto ix = interpolationSearch(tid, key, node);
//        if (node->ptr(ix) & 1) { //////////// debug
//            std::cout<<"ix="<<ix<<std::endl;
//            std::cout<<"node @ "<<(size_t) node<<" node degree="<<node->degree<<" pointers to";
//            for (int i=0;i<node->degree;++i) std::cout<<" "<<(size_t) node->ptr(i);
//            std::cout<<std::endl;
//            std::cout<<"key="<<key<<" node minKey="<<node->minKey<<" maxKey="<<node->maxKey<<std::endl;
//            
//            std::cout<<"addr of node="<<(size_t) node<<std::endl;
//            std::cout<<"offset of capacity=0"<<std::endl;
//            std::cout<<"offset of degree="<<((size_t) &node->degree) - ((size_t) node)<<std::endl;
//            std::cout<<"offset of initSize="<<((size_t) &node->initSize) - ((size_t) node)<<std::endl;
//            std::cout<<"offset of changeSum="<<((size_t) &node->changeSum) - ((size_t) node)<<std::endl;
//            std::cout<<"offset of dirty="<<((size_t) &node->dirty) - ((size_t) node)<<std::endl;
//            std::cout<<"offset of parent="<<((size_t) &node->parent) - ((size_t) node)<<std::endl;
//            std::cout<<"offset of minKey="<<((size_t) &node->minKey) - ((size_t) node)<<std::endl;
//            std::cout<<"offset of maxKey="<<((size_t) &node->maxKey) - ((size_t) node)<<std::endl;
//            if (node->degree >= 2) std::cout<<"offset of key0="<<((size_t) node->keyAddr(0)) - ((size_t) node)<<std::endl;
//            if (node->degree >= 1) std::cout<<"offset of ptr0="<<((size_t) node->ptrAddr(0)) - ((size_t) node)<<std::endl;
//            if (node->degree >= 2) std::cout<<"offset of ptr1="<<((size_t) node->ptrAddr(1)) - ((size_t) node)<<std::endl;
//            std::cout<<"offset of end of node="<<((size_t) (((char *) &node->maxKey) + sizeof(node->maxKey) + (sizeof(K) * (node->capacity - 1) + sizeof(casword_t) * node->capacity))) - ((size_t) node)<<std::endl;
//
//            // bfs debug print
//            std::vector<Node<K,V>*> q;
//            q.push_back(root);
//            int k=0;
//            while (k < q.size()) {
//                auto node = q[k++]; // pop
//                if (node == NULL) {
//                    std::cout<<"null"<<std::endl;
//                    continue;
//                }
//                std::cout<<" degree="<<node->degree;
//                for (int i=0;i<node->degree-1;++i) {
//                    std::cout<<(i==0?":":",")<<node->key(i);
//                }
//                std::cout<<" @ "<<(size_t) node<<std::endl;
//                std::cout<<" ptrs";
//                for (int i=0;i<node->degree;++i) {
//                    std::cout<<(i==0?":":",")<<(size_t)(node->ptr(i)&~0x7)<<(node->ptr(i)&0x1?"d":node->ptr(i)&0x2?"k":node->ptr(i)&0x4?"r":"n");
//                }
//                std::cout<<std::endl;
//
//                for (int i=0;i<node->degree;++i) {
//                    auto ptr = node->ptr(i);
//                    if (IS_KVPAIR(ptr)) {
//
//                    } else if (IS_REBUILDOP(ptr)) {
//
//                    } else {
//                        assert(IS_NODE(ptr));
//                        q.push_back(CASWORD_TO_NODE(ptr));
//                    }
//                }
//            }
//            assert(false);
//        } //////////// debug
        auto word = prov->readPtr(tid, node->ptrAddr(ix));
        if (IS_KVPAIR(word)) {
            auto pair = CASWORD_TO_KVPAIR(word);
            
            V retval = NO_VALUE;
            auto newWord = (casword_t) NULL;
            KVPair<K,V> * newPair = NULL;
            switch (t) {
                case InsertReplace: case InsertIfAbsent:
                    if (pair->k == INF_KEY) {
                        newWord = KVPAIR_TO_CASWORD(createKVPair(tid, key, val));
                        // retval = NO_VALUE;
                    } else if (pair->k == key) {
                        if (t == InsertIfAbsent) return pair->v;
                        newWord = KVPAIR_TO_CASWORD(createKVPair(tid, key, val));
                        retval = pair->v;
                    } else {
                        auto newPair = createKVPair(tid, key, val);
                        auto newNode = createNode(tid, 2);
                        newNode->degree = 2;
                        newNode->initSize = 2;
                        newNode->parent = node;
                        
                        if (key < pair->k) {
                            newNode->key(0) = pair->k;
                            *newNode->ptrAddr(0) = KVPAIR_TO_CASWORD(newPair);
                            *newNode->ptrAddr(1) = KVPAIR_TO_CASWORD(pair);
                        } else {
                            newNode->key(0) = key;
                            *newNode->ptrAddr(0) = KVPAIR_TO_CASWORD(pair);
                            *newNode->ptrAddr(1) = KVPAIR_TO_CASWORD(newPair);
                        }
                        newNode->minKey = newNode->key(0);
                        newNode->maxKey = newNode->key(0);
                        newWord = NODE_TO_CASWORD(newNode);
                        // retval = NO_VALUE;
                    }
                    break;
                case Erase:
                    if (pair->k != key) return NO_VALUE;
                    newPair = createKVPair(tid, INF_KEY, NO_VALUE);
                    newPair->empty = true;
                    newWord = KVPAIR_TO_CASWORD(newPair);
                    retval = pair->v;
                    break;
                default:
                    setbench_error("impossible switch case");
                    break;
            }
            assert(newWord);
            
            // DCSS that performs the update
//            *node->ptrAddr(ix) = newWord;
            auto result = prov->dcssPtr(tid, (casword_t *) &node->dirty, 0, node->ptrAddr(ix), word, newWord);
            switch (result.status) {
                case DCSS_FAILED_ADDR2: // retry from same node
                    goto retryNode;
                    break;
                case DCSS_FAILED_ADDR1: // node is dirty; retry from root
                    goto retry;
                    break;
                case DCSS_SUCCESS:
                    
                    // probabilistically increment the changeSums of ancestors
                    if (myRNG->next() / (double) std::numeric_limits<uint64_t>::max() < SAMPLING_PROB) {
                        for (int i=0;i<pathLength;++i) {
                            __sync_fetch_and_add(&path[i]->changeSum, 1);
                        }
                    }
                    
                    // now, we must determine whether we should rebuild
                    for (int i=0;i<pathLength;++i) {
                        if ((path[i]->changeSum + NUM_PROCESSES) / (SAMPLING_PROB * (1 - EPS)) >= REBUILD_FRACTION * path[i]->initSize) {
                            auto parent = (i == 0) ? root : path[i-1];
                            auto index = (path[i]->degree == 1) ? 0 : interpolationSearch(tid, path[i]->key(0), parent);
                            rebuild(tid, path[i], parent, index);
                            break;
                        }
                    }
                    
                    break;
                default:
                    setbench_error("impossible switch case");
                    break;
            }
            return retval;
        } else if (IS_REBUILDOP(word)) {
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
    Node<K,V> * node = (Node<K,V> *) new char[sz];
//    std::cout<<"node of degree "<<degree<<" allocated size "<<sz<<" @ "<<(size_t) node<<std::endl;
    assert((((size_t) node) & 0x7) == 0);
    node->capacity = degree;
    node->degree = 0;
    node->initSize = 0;
    node->changeSum = 0;
    node->dirty = 0;
    node->parent = NULL;
    assert(node);
    if (degree == 1) assert(sz + (size_t) node >= (size_t) node->ptrAddr(0));
    if (degree == 2) assert(sz + (size_t) node >= (size_t) node->ptrAddr(1));
    return node;
}

template <typename K, typename V, class Interpolate, class RecManager>
KVPair<K,V> * istree<K,V,Interpolate,RecManager>::createKVPair(const int tid, const K& key, const V& value) {
    auto result = new KVPair<K,V>(key, value);
//    std::cout<<"kvpair allocated of size "<<sizeof(KVPair<K,V>)<<" with key="<<key<<" @ "<<(size_t) result<<std::endl;
    assert((((size_t) result) & 0x7) == 0);
    assert(result);
    return result;
}

#endif /* BROWN_EXT_IST_LF_IMPL_H */
