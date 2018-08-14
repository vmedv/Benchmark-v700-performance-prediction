/**
 * -----------------------------------------------------------------------------
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ISTREE_H
#define	ISTREE_H

#define PREFILL_SEQUENTIAL_BUILD_FROM_ARRAY

#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <limits>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <sys/types.h>
#include "random_fnv1a.h"
#include "record_manager.h"
#include "dcss_impl.h"

#define KVPAIR_MASK (0x2) /* 0x1 is used by DCSS */
#define IS_KVPAIR(x) ((x)&KVPAIR_MASK)
#define CASWORD_TO_KVPAIR(x) ((KVPair<K,V> *) ((x)&~KVPAIR_MASK))
#define KVPAIR_TO_CASWORD(x) ((casword_t) (((casword_t) (x))|KVPAIR_MASK))

#define REBUILDOP_MASK (0x4)
#define IS_REBUILDOP(x) ((x)&REBUILDOP_MASK)
#define CASWORD_TO_REBUILDOP(x) ((RebuildOperation<K,V> *) ((x)&~REBUILDOP_MASK))
#define REBUILDOP_TO_CASWORD(x) ((casword_t) (((casword_t) (x))|REBUILDOP_MASK))

#define IS_NODE(x) (((x)&(REBUILDOP_MASK|KVPAIR_MASK))==0)
#define CASWORD_TO_NODE(x) ((Node<K,V> *) (x))
#define NODE_TO_CASWORD(x) ((casword_t) (x))

#define SUM_TO_DIRTY(x) (((x)<<1)|1)
#define DIRTY_TO_SUM(x) ((x)>>1)

#define REBUILD_FRACTION (0.25)
#define EPS REBUILD_FRACTION

static __thread RandomFNV1A * myRNG = NULL;

enum UpdateType {
    InsertIfAbsent, InsertReplace, Erase
};

template <typename K, typename V>
struct Node {
    size_t capacity;
    size_t degree;
    size_t initSize;
    size_t changeSum;
    size_t dirty;
    Node<K,V> * parent;
    K minKey;
    K maxKey;
    // unlisted fields: capacity-1 keys of type K followed by capacity "pointers" of type casword_t
    
    inline K * keyAddr(const int ix) {
        K * const firstKey = ((K *) ((&maxKey)+1));
        return &firstKey[ix];
    }
    inline K& key(const int ix) {
        return *keyAddr(ix);
    }
    // conceptually returns &node.ptrs[ix]
    inline casword_t * ptrAddr(const int ix) {
        K * const firstKeyAfter = keyAddr(capacity - 1);
        casword_t * const firstPtr = (casword_t *) firstKeyAfter;
        return &firstPtr[ix];
    }

    // conceptually returns node.ptrs[ix]
    inline casword_t ptr(const int ix) {
        return *ptrAddr(ix);
    }
};

template <typename K, typename V>
struct RebuildOperation {
    Node<K,V> * candidate;
    Node<K,V> * parent;
    size_t index;
    RebuildOperation(Node<K,V> * _candidate, Node<K,V> * _parent, size_t _index)
        : candidate(_candidate), parent(_parent), index(_index) {}
};

template <typename K, typename V>
struct KVPair {
    K k;
    V v;
    bool empty;
    KVPair(const K& key, const V& value)
    : k(key), v(value), empty(false) {}
};

template <typename K, typename V, class Interpolate, class RecManager>
class istree {
private:
    PAD;
    RecManager * const recordmgr;
    dcssProvider * const prov;
    Interpolate cmp;

    Node<K,V> * root;

    #define arraycopy(src, srcStart, dest, destStart, len) \
        for (int ___i=0;___i<(len);++___i) { \
            (dest)[(destStart)+___i] = (src)[(srcStart)+___i]; \
        }

    size_t markAndCount(const int tid, const casword_t ptr);
    void rebuild(const int tid, Node<K,V> * rebuildRoot, Node<K,V> * parent, int index);
    void helpRebuild(const int tid, RebuildOperation<K,V> * op);
    int interpolationSearch(const int tid, const K& key, Node<K,V> * const node);
    V doUpdate(const int tid, const K& key, const V& val, UpdateType t);

    Node<K,V> * createNode(const int tid, const int degree);
    KVPair<K,V> * createKVPair(const int tid, const K& key, const V& value);

    class IdealBuilder {
    private:
        // figure out how deep the tree should be,
        // and how many keys should be stored in each node, at each level, on average
        static const int UPPER_LIMIT_DEPTH = 16;
        static const int MAX_ACCEPTABLE_LEAF_SIZE = 48;
        int height;
        size_t initNumKeys;
        double init;
        double keysPerNodeAtDepth[UPPER_LIMIT_DEPTH];
        double totalDegreeAtDepthFractional[UPPER_LIMIT_DEPTH];
        size_t totalDegreeAtDepth[UPPER_LIMIT_DEPTH];
        Node<K,V> * currNodeAtDepth[UPPER_LIMIT_DEPTH];
        istree<K,V,Interpolate,RecManager> * ist;
        size_t keysAdded; // for debugging
    
    public:
        IdealBuilder(const size_t _initNumKeys, istree<K,V,Interpolate,RecManager> * _ist) {
            height = -1;
            initNumKeys = _initNumKeys;
            
            if (initNumKeys <= MAX_ACCEPTABLE_LEAF_SIZE) {
                keysPerNodeAtDepth[0] = initNumKeys;
                height = 1;

            } else {
                double init = _initNumKeys;
                for (int i=0;i<UPPER_LIMIT_DEPTH;++i) {
                    init = std::sqrt(init);
                    keysPerNodeAtDepth[i] = init;
                    if ((size_t) keysPerNodeAtDepth[i] <= MAX_ACCEPTABLE_LEAF_SIZE) {
                        height = i+2; // as we explain below, we will add one extra level for leaves
                        break;
                    }
                }
                // the product of the tail sizes is equal to the previous square
                // so we truncate the tail by making leaves equal in size
                // to the previous level of internal nodes.
                assert(height >= 2);
                //if (height >= 2) {
                    keysPerNodeAtDepth[height-1] = keysPerNodeAtDepth[height-2];
                //}
            }

//            std::cout<<"initNumKeys="<<initNumKeys<<std::endl;
//            std::cout<<"height="<<height<<std::endl;
//            for (int i=0;i<height;++i) {
//                std::cout<<"keysPerNodeAtDepth["<<i<<"]="<<keysPerNodeAtDepth[i]<<std::endl;
//            }
            
            // we will build left to right, starting from the bottom, and building all levels at once.
            // for each level, create a "current node" that is in the process of being constructed.
            for (int i=0;i<height;++i) {
                currNodeAtDepth[i] = NULL;
                totalDegreeAtDepthFractional[i] = 0;
                totalDegreeAtDepth[i] = 0;
            }

            ist = _ist;
            keysAdded = 0;
        }
        
        void addKV(const int tid, const K& key, const V& val) {
            ++keysAdded;
            if (keysAdded > initNumKeys) {
                setbench_error("added "<<keysAdded<<" keys, when there were supposed to be only "<<initNumKeys);
            }
            
            // figure out if any nodes are full (or NULL)
            //      go bottom up, until you hit a non-empty node
            //      we will replace all nodes below that
            int top=-1;
            for (int i=height-1;i>=0;--i) {
                if (currNodeAtDepth[i] == NULL || currNodeAtDepth[i]->degree == currNodeAtDepth[i]->capacity) {
                    top=i;
                } else {
                    break;
                }
            }
            // assert: parent of top is not full (or top is root)
            assert(top <= 0 || currNodeAtDepth[top-1]->degree < currNodeAtDepth[top-1]->capacity);
            
            // replace all nodes from top to height-1
            if (top != -1) {
                
                // bottom up: push all initSize values to ancestors
                // (ending at the parent still in currNodeAtDepth...
                //  its value will be pushed when is replaced)
                for (int i=height-1;i>=top;--i) {
                    if (i > 0) {
                        auto p = currNodeAtDepth[i-1];
                        auto curr = currNodeAtDepth[i];
                        if (p && curr) p->initSize += curr->initSize;
                    }
                }
                
                // top down: create new nodes of appropriate (random) sizes
                for (int i=top;i<height;++i) {
                    totalDegreeAtDepthFractional[i] += keysPerNodeAtDepth[i];
                    size_t nodeDegree = (size_t) keysPerNodeAtDepth[i];
                    if (totalDegreeAtDepth[i] + nodeDegree < totalDegreeAtDepthFractional[i]) {
                        ++nodeDegree;
                    }
                    totalDegreeAtDepth[i] += nodeDegree;

                    /** BEGIN DEBUGGING CODE **/
                    if (i==0 && currNodeAtDepth[0]) {
                        std::cout<<"keysAdded="<<keysAdded<<std::endl;
                        std::cout<<"initNumKeys="<<initNumKeys<<std::endl;
                        std::cout<<"height="<<height<<std::endl;
                        for (int j=0;j<height;++j) std::cout<<"keysPerNodeAtDepth["<<j<<"]="<<keysPerNodeAtDepth[j]<<std::endl;
                        for (int j=0;j<height;++j) std::cout<<"totalDegreeAtDepthFractional["<<j<<"]="<<totalDegreeAtDepthFractional[j]<<std::endl;
                        for (int j=0;j<height;++j) std::cout<<"totalDegreeAtDepth["<<j<<"]="<<totalDegreeAtDepth[j]<<std::endl;
                        std::cout<<"capacity of root="<<currNodeAtDepth[i]->capacity<<std::endl;

                        // bfs to get node sizes
                        std::vector<Node<K,V> *> q;
                        std::vector<int> qq;
                        q.push_back(currNodeAtDepth[i]);
                        qq.push_back(0);
                        int k=0;
                        while (k < q.size()) {
                            auto node = q[k];
                            auto x = qq[k];
                            ++k;

                            std::cout<<"node@"<<(size_t) node<<" depth="<<x<<" capacity="<<node->capacity<<std::endl;
                            for (int j=0;j<node->degree;++j) {
                                auto ptr = node->ptr(j); //prov->readPtr(tid, node->ptrAddr(j));
                                if (IS_NODE(ptr)) {
                                    q.push_back(CASWORD_TO_NODE(ptr));
                                    qq.push_back(x+1);
                                }
                            }
                        }
                        std::cout<<"ERROR: non-null root is replaced unexpectedly during rebuilding!"<<std::endl;
                        assert(false);
                    }
                    /** END DEBUGGING CODE **/
                    
                    auto curr = ist->createNode(tid, nodeDegree);
                    currNodeAtDepth[i] = curr;
                    
                    // link the new node to the existing parent
                    if (i > 0) {
                        auto p = currNodeAtDepth[i-1];
                        curr->parent = p;
                        auto pdeg = p->degree;
                        if (pdeg > 0) {
                            p->key(pdeg-1) = key;
                            if (pdeg == 1) {
                                p->minKey = key;
                            }
                            p->maxKey = key;
                        }
                        *p->ptrAddr(pdeg) = NODE_TO_CASWORD(curr);
                        ++p->degree;
                    }
                }
            }
            
            // insert key and value
            auto kvptr = ist->createKVPair(tid, key, val);
            
            // insert kvpair into current open bottom level node
            auto node = currNodeAtDepth[height-1];
            assert(node->degree < node->capacity); // assert: bottom level current node is not full
            auto deg = node->degree;
            if (deg > 0) {
                node->key(deg-1) = key;
                if (deg == 1) {
                    node->minKey = key;
                }
                node->maxKey = key;
            }
            *node->ptrAddr(deg) = KVPAIR_TO_CASWORD(kvptr);
            assert(kvptr);
            ++node->initSize;
            ++node->degree;
        }
        
        Node<K,V> * getRoot() {
            return currNodeAtDepth[0];
        }
    };
    void createIdeal(const int tid, casword_t ptr, IdealBuilder * b);
    Node<K,V> * createIdeal(const int tid, Node<K,V> * rebuildRoot, const size_t keyCount);
    
    int init[MAX_THREADS_POW2] = {0,};
public:
    const K INF_KEY;
    const V NO_VALUE;
    const int NUM_PROCESSES;
    PAD;

    void initThread(const int tid) {
        if (myRNG == NULL) myRNG = new RandomFNV1A(rand());
        if (init[tid]) return; else init[tid] = !init[tid];

        prov->initThread(tid);
        recordmgr->initThread(tid);
    }
    void deinitThread(const int tid) {
        if (myRNG != NULL) { delete myRNG; myRNG = NULL; }
        if (!init[tid]) return; else init[tid] = !init[tid];

        prov->deinitThread(tid);
        recordmgr->deinitThread(tid);
    }

    istree(const int numProcesses
         , const K infinity
         , const V noValue
    )
    : recordmgr(new RecManager(numProcesses, SIGQUIT))
    , prov(new dcssProvider(numProcesses))
    , INF_KEY(infinity)
    , NO_VALUE(noValue)
    , NUM_PROCESSES(numProcesses) 
    {
        srand(time(0)); // for seeding per-thread RNGs in initThread
        cmp = Interpolate();

        const int tid = 0;
        initThread(tid);

        Node<K,V> * _root = createNode(tid, 1);
        KVPair<K,V> * _rootPair = createKVPair(tid, INF_KEY, NO_VALUE);
        _root->degree = 1;
        _root->minKey = INF_KEY;
        _root->maxKey = INF_KEY;
        *_root->ptrAddr(0) = KVPAIR_TO_CASWORD(_rootPair);
        
        root = _root;
    }

    istree(const K * const initKeys
         , const V * const initValues
         , const size_t initNumKeys
         , const size_t initConstructionSeed /* note: randomness is used to ensure good tree structure whp */
         , const int numProcesses
         , const K infinity
         , const V noValue
    )
    : recordmgr(new RecManager(numProcesses, SIGQUIT))
    , prov(new dcssProvider(numProcesses))
    , INF_KEY(infinity)
    , NO_VALUE(noValue)
    , NUM_PROCESSES(numProcesses) 
    {
        cmp = Interpolate();

        const int tid = 0;
        initThread(tid);

        Node<K,V> * _root = createNode(tid, 1);
        _root->degree = 1;
        root = _root;
        
        // build ideal initial tree
        // note: myRNG must be created by initThread before creating IdealBuilder!!!
        IdealBuilder b (initNumKeys, this);
        for (size_t keyIx=0;keyIx<initNumKeys;++keyIx) {
            b.addKV(tid, initKeys[keyIx], initValues[keyIx]);
        }
        *root->ptrAddr(0) = NODE_TO_CASWORD(b.getRoot());
        assert(b.getRoot());
                
        std::cout<<"istree created with NUM_PROCESSES="<<NUM_PROCESSES<<std::endl;
    }

    ~istree() {
//        int nodes = 0;
//        freeSubtree(root, &nodes);
////            COUTATOMIC("main thread: deleted tree containing "<<nodes<<" nodes"<<std::endl);
////            recordmgr->printStatus();
        delete recordmgr;
    }

    Node<K,V> * debug_getEntryPoint() { return root; }

    V find(const int tid, const K& key);
    bool contains(const int tid, const K& key) {
        return find(tid, key);
    }
    V insert(const int tid, const K& key, const V& val) {
        return doUpdate(tid, key, val, InsertReplace);
    }
    V insertIfAbsent(const int tid, const K& key, const V& val) {
        return doUpdate(tid, key, val, InsertIfAbsent);
    }
    V erase(const int tid, const K& key) {
        return doUpdate(tid, key, NO_VALUE, Erase);
    }
    RecManager * const debugGetRecMgr() {
        return recordmgr;
    }
};

#endif	/* ISTREE_H */
