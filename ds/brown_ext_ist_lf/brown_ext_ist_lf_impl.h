#ifndef BROWN_EXT_IST_LF_IMPL_H
#define BROWN_EXT_IST_LF_IMPL_H

#include "brown_ext_ist_lf.h"

template <typename K, typename V, class Interpolate, class RecManager>
const std::pair<V,bool> istree<K,V,Interpolate,RecManager>::erase(const int tid, const K& key) {
    return std::pair<V,bool>(NO_VALUE, false);
}

template <typename K, typename V, class Interpolate, class RecManager>
const std::pair<V,bool> istree<K,V,Interpolate,RecManager>::find(const int tid, const K& key) {
    casword_t ptr = prov->readPtr(tid, root->ptrAddr(0));
    assert(ptr);
    while (true) {
        if (IS_KVPAIR(ptr)) {
            auto kv = CASWORD_TO_KVPAIR(ptr);
            if (kv->k == key) return std::pair<V,bool>(kv->v, true);
            return std::pair<V,bool>(NO_VALUE, false);
        } else if (IS_REBUILDOP(ptr)) {
            auto rebuild = CASWORD_TO_REBUILDOP(ptr);
            ptr = NODE_TO_CASWORD(rebuild->candidate);
        } else {
            assert(IS_NODE(ptr));
            assert(ptr);
            // ptr is an internal node
            auto node = CASWORD_TO_NODE(ptr);
            const int ix = interpolationSearch(key, node);
            ptr = prov->readPtr(tid, node->ptrAddr(ix));
        }
    }
}

template <typename K, typename V, class Interpolate, class RecManager>
bool istree<K,V,Interpolate,RecManager>::contains(const int tid, const K& key) {
    return find(tid, key).second;
}

template <typename K, typename V, class Interpolate, class RecManager>
size_t istree<K,V,Interpolate,RecManager>::dfsMark(const int tid, const casword_t ptr) {
    if (IS_KVPAIR(ptr)) {
        return 1;
    }
    if (IS_REBUILDOP(ptr)) {
        auto op = CASWORD_TO_REBUILDOP(ptr);
        return dfsMark(tid, NODE_TO_CASWORD(op->candidate));
    }
    auto node = CASWORD_TO_NODE(ptr);
    
    __sync_bool_compare_and_swap(&node->dirty, 0, 1);
    
    size_t sum = 0;
    for (int i=0;i<node->degree;++i) {
        sum += dfsMark(tid, prov->readPtr(tid, node->ptrAddr(i)));
    }
    
    if (sum) __sync_bool_compare_and_swap(&node->dirty, 1, SUM_TO_DIRTY(sum));
    return sum;
}

template <typename K, typename V, class Interpolate, class RecManager>
Node<K,V> * istree<K,V,Interpolate,RecManager>::dfsBuild(const int tid, Node<K,V> ** const currAtDepth, const int buildDepth) {
    return NULL;
}

template <typename K, typename V, class Interpolate, class RecManager>
void istree<K,V,Interpolate,RecManager>::helpRebuildSubtree(const int tid, RebuildOperation<K,V> * op) {
    // dfs to mark nodes and compute up to date subtree sizes
    size_t size = dfsMark(tid, NODE_TO_CASWORD(op->candidate));
    
    /**
     * rebuild subtree
     */
    
    
    /**
     * TODO: redo with tuned probabilistic tree building
     */
    
    // compute lookup table
    const int MAX_DEPTH = 9; // repeated sqrt on 2^64 hits 1 after 7 iterations.
    const int MIN_NODE_DEGREE = 4;
    size_t nodeDegreeAtDepth[MAX_DEPTH];
    nodeDegreeAtDepth[0] = (size_t) std::ceil(std::pow((double) size, 0.5));
    int buildDepth = 1;
    for (int i=1;i<MAX_DEPTH;++i) {
        nodeDegreeAtDepth[i] = (size_t) std::ceil(std::pow((double) nodeDegreeAtDepth[i-1], 0.5));
        if (nodeDegreeAtDepth[i] >= MIN_NODE_DEGREE) ++buildDepth;
    }
    
    // fold any levels with degree <= 3 into the last level
    assert(buildDepth < MAX_DEPTH);
    if (nodeDegreeAtDepth[buildDepth] == 3) {
        nodeDegreeAtDepth[buildDepth - 1] *= 6;
    } else {
        nodeDegreeAtDepth[buildDepth - 1] *= 2;
    }
    
    // build tree left to right with array of current/"open" nodes and a dfs
    Node<K,V> * currAtDepth[MAX_DEPTH];
    for (int i=0;i<buildDepth;++i) {
        currAtDepth[i] = createNode(tid, nodeDegreeAtDepth[i]);
        if (i > 0) currAtDepth[i]->parent = currAtDepth[i-1];
    }
    auto node = dfsBuild(tid, currAtDepth, buildDepth);
    
    // replace subtree and update parent counter
    
}

template <typename K, typename V, class Interpolate, class RecManager>
void istree<K,V,Interpolate,RecManager>::rebuildSubtree(const int tid, PropagateResult<K,V> result) {
    RebuildOperation<K,V> * op = new RebuildOperation<K,V>();
    op->candidate = result.candidate;
    op->parent = op->candidate->parent;
    op->index = interpolationSearch(op->candidate->key(0), op->parent);
    casword_t ptr = REBUILDOP_TO_CASWORD(op);
    casword_t old = NODE_TO_CASWORD(op->candidate);
    
    dcssresult_t dcssResult = prov->dcssPtr(tid, &op->parent->dirty, 0, op->parent->ptrAddr(op->index), old, ptr);
    switch (dcssResult.status) {
        case DCSS_FAILED_ADDR2: case DCSS_FAILED_ADDR1: {
            break; // we are done
        }
        case DCSS_SUCCESS: {
            helpRebuildSubtree(tid, op);
            break;
        }
        default: {
            setbench_error("executed default switch case");
            break;
        }
    }
}

template <typename K, typename V, class Interpolate, class RecManager>
V istree<K,V,Interpolate,RecManager>::doInsert(const int tid, const K& key, const V& value, const bool replace) {
restart:
    auto retval = doInsert(tid, root, key, value, replace);
    switch (retval.type) {
        case RESTART:
            goto restart;
            break;
        case PROPAGATE:
            if (retval.prop.candidate) {
                rebuildSubtree(tid, retval.prop);
            }
            return retval.prop.oldValue;
            break;
        case NO_PROPAGATE:
            setbench_error("impossible case")
            break;
        case DONE:
            return retval.noprop.oldValue;
            break;
        default:
            setbench_error("default case should never execute");
            break;
    }
    setbench_error("passed switch illegally");
}

template <typename K, typename V, class Interpolate, class RecManager>
int istree<K,V,Interpolate,RecManager>::interpolationSearch(const K& key, Node<K,V> * const node) {
    assert(node->degree >= 1);
    if (node->degree == 1) return 0;
    
    const int numKeys = node->degree - 1;
    const K& minKey = node->key(0);
    const K& maxKey = node->key(numKeys-1);
    double pos = cmp.interpolate(key, minKey, maxKey);
    pos = (pos < 0) ? 0 : (pos > 1) ? 1 : pos;
    
    int ix = (int) (pos * numKeys);
    int c = cmp.compare(key, node->key(ix));
    
    if (c < 0) {
        // search to the left for node.key[i] <= key, then return i+1
        for (int i=ix-1;i>=0;--i) {
            c = cmp.compare(key, node->key(i));
            if (c >= 0) return i+1;
        }
        return 0;
    } else if (c > 0) {
        // search to the right for node.key[ix] > key, then return ix
        for (int i=ix+1;i<numKeys;++i) { // recall: degree - 1 keys vs degree pointers
            c = cmp.compare(key, node->key(i));
            if (c < 0) return i;
        }
        return numKeys;
    } else { // c == 0
        return ix+1;
    }
}

template <typename K, typename V, class Interpolate, class RecManager>
InsertResult<K,V> istree<K,V,Interpolate,RecManager>::doInsert(const int tid, Node<K,V> * const node, const K& key, const V& value, const bool replace) {
    // search for child pointer
    
    const int ix = interpolationSearch(key, node);
    casword_t childPtr = prov->readPtr(tid, node->ptrAddr(ix));
    
    // check for leaf / rebuild op

    InsertResult<K,V> retval;
    InsertResult<K,V> newInsResult;
    dcssresult_t dcssResult;
    if (IS_KVPAIR(childPtr)) {
        // prep retval as if we'd recursively called doInsert (mimicking the case below)
        
        retval.type = NO_PROPAGATE;
        KVPair<K,V> * pair = CASWORD_TO_KVPAIR(childPtr);
        
        if (pair->k == key) {
            retval.noprop.oldValue = pair->v;
            if (replace) {
                KVPair<K,V> * newPair = createKVPair(tid, key, value);
                retval.noprop.newObject = KVPAIR_TO_CASWORD(newPair);
            } else {
                retval.noprop.newObject = (casword_t) NULL;
            }
            
        } else {
            KVPair<K,V> * newPair = createKVPair(tid, key, value);
            Node<K,V> * newNode = createNode(tid, 2);
            newNode->degree = 2;
            newNode->initSize = 2;
            newNode->parent = node;
            
            if (key < pair->k) {
                newNode->key(0) = (K) 0;
                newNode->key(0) = pair->k;
                *(newNode->ptrAddr(0)) = KVPAIR_TO_CASWORD(newPair);
                *(newNode->ptrAddr(1)) = KVPAIR_TO_CASWORD(pair);
            } else {
                newNode->key(0) = key;
                *(newNode->ptrAddr(0)) = KVPAIR_TO_CASWORD(pair);
                *(newNode->ptrAddr(1)) = KVPAIR_TO_CASWORD(newPair);
            }
            
            retval.noprop.newObject = (casword_t) newNode;
            retval.noprop.oldValue = NO_VALUE;
        }
    } else if (IS_REBUILDOP(childPtr)) {
        helpRebuildSubtree(tid, CASWORD_TO_REBUILDOP(childPtr));
        newInsResult.type = RESTART;
        return newInsResult;
        
    } else {
        // try to insert what we now know is a Node

        Node<K,V> * child = CASWORD_TO_NODE(childPtr);
        retval = doInsert(tid, child, key, value, replace);
    }
    
    switch (retval.type) {
        case RESTART:
            return retval;
            break;
        case NO_PROPAGATE:
            if (retval.noprop.newObject == (casword_t) NULL) {
                newInsResult.type = DONE;
                newInsResult.noprop.oldValue = retval.noprop.oldValue;
                return newInsResult;
            }
            
            dcssResult = prov->dcssPtr(tid, &node->dirty, 0, node->ptrAddr(ix), childPtr, retval.noprop.newObject);
            switch (dcssResult.status) {
                case DCSS_FAILED_ADDR2: {
                    return doInsert(tid, node, key, value, replace);
                    break;
                }
                case DCSS_FAILED_ADDR1: { // node is dirty
                    newInsResult.type = RESTART;
                    return newInsResult;
                    break;
                }
                case DCSS_SUCCESS: {
                    newInsResult.type = PROPAGATE;
                    if (node == root) {
                        newInsResult.prop = PropagateResult<K,V>(NULL, 0, node->changeSum, retval.noprop.oldValue);
                        return newInsResult;
                    }
                    __sync_fetch_and_add(&node->changeSum, 1);
                    if (node->changeSum > REBUILD_AFTER_CHANGE_FRACTION * node->initSize) {
                        newInsResult.prop = PropagateResult<K,V>(node, node->changeSum, node->changeSum, retval.noprop.oldValue);
                    } else {
                        newInsResult.prop = PropagateResult<K,V>(NULL, 0, node->changeSum, retval.noprop.oldValue);
                    }
                    return newInsResult;
                    break;
                }
                default: {
                    setbench_error("executed default switch case");
                    break;
                }
            }
            break;
        case PROPAGATE:
            newInsResult.type = PROPAGATE;
            if (node->changeSum + retval.prop.childChangeSum > REBUILD_AFTER_CHANGE_FRACTION * node->initSize) {
                newInsResult.prop = PropagateResult<K,V>(node, node->changeSum + retval.prop.childChangeSum, node->changeSum + retval.prop.childChangeSum, retval.noprop.oldValue);
            } else {
                newInsResult.prop = PropagateResult<K,V>(retval.prop.candidate, retval.prop.childChangeSum, node->changeSum + retval.prop.childChangeSum, retval.noprop.oldValue);
            }
            return newInsResult;
            break;
        case DONE:
            return retval;
            break;
        default:
            std::cout<<"retval.type="<<retval.type<<std::endl;
            std::cout<<"RESTART="<<RESTART<<" PROPAGATE="<<PROPAGATE<<" NO_PROPAGATE="<<NO_PROPAGATE<<" DONE="<<DONE<<std::endl;
            setbench_error("default case should never execute");
            break;
    }
    
    InsertResult<K,V> result;
    result.type = RESTART;
    return result;
}

template <typename K, typename V, class Interpolate, class RecManager>
Node<K,V>* istree<K,V,Interpolate,RecManager>::createNode(const int tid, const int degree) {
//    auto sz = sizeof(Node<K,V>) - sizeof(void *) + sizeof(K) * (degree - 1) + sizeof(casword_t) * degree;
//    std::cout<<"size of allocated node = "<< sz << std::endl;
    Node<K,V> * node = (Node<K,V> *) new char[sizeof(Node<K,V>) - sizeof(void *) + sizeof(K) * (degree - 1) + sizeof(casword_t) * degree];
    node->capacity = degree;
    node->degree = 0;
    node->initSize = 0;
    node->changeSum = 0;
    node->dirty = 0;
    node->parent = NULL;
    return node;
}

template <typename K, typename V, class Interpolate, class RecManager>
KVPair<K,V> * istree<K,V,Interpolate,RecManager>::createKVPair(const int tid, const K& key, const V& value) {
    return new KVPair<K,V>(key, value);
}

#endif /* BROWN_EXT_IST_LF_IMPL_H */

