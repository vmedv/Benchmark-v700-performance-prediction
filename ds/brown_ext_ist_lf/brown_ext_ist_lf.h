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

private:
//    Node<K,V> * dfsBuild(const int tid, Node<K,V> ** const currAtDepth, const int buildDepth);
//    size_t dfsMark(const int tid, const casword_t ptr);
    void rebuild(const int tid, Node<K,V> * rebuildRoot, Node<K,V> * parent, int index);
    void helpRebuild(const int tid, RebuildOperation<K,V> * op);
//    V doInsert(const int tid, const K& key, const V& value, const bool replace);
    int interpolationSearch(const int tid, const K& key, Node<K,V> * const node);
    V doUpdate(const int tid, const K& key, const V& val, UpdateType t);

    Node<K,V> * createNode(const int tid, const int degree);
    KVPair<K,V> * createKVPair(const int tid, const K& key, const V& value);

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
//        srand(187381781); // for seeding per-thread RNGs in initThread
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
        
        /**
         * build ideal initial tree
         */
        
        RandomFNV1A rng (initConstructionSeed);
        
        // figure out how deep the tree should be,
        // and how many keys should be stored in each node, at each level, on average
        const int UPPER_LIMIT_DEPTH = 16;
        const int MAX_ACCEPTABLE_LEAF_SIZE = 48;
        double init = initNumKeys;
        double keysPerNodeAtDepth[UPPER_LIMIT_DEPTH];
        size_t floorKeysPerNodeAtDepth[UPPER_LIMIT_DEPTH];
        int height = -1;
        for (int i=0;i<UPPER_LIMIT_DEPTH;++i) {
            init = std::sqrt(init);
            keysPerNodeAtDepth[i] = init;
            floorKeysPerNodeAtDepth[i] = (size_t) keysPerNodeAtDepth[i];
            if (floorKeysPerNodeAtDepth[i] < MAX_ACCEPTABLE_LEAF_SIZE) {
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
            floorKeysPerNodeAtDepth[height-1] = floorKeysPerNodeAtDepth[height-2];
        //}
        assert(height != -1);
        
        std::cout<<"initNumKeys="<<initNumKeys<<std::endl;
        std::cout<<"height="<<height<<std::endl;
        for (int i=0;i<height;++i) {
            std::cout<<"keysPerNodeAtDepth["<<i<<"]="<<keysPerNodeAtDepth[i]<<std::endl;
        }
        
        // build left to right, starting from the bottom, and building all levels at once.
        // for each level, create a "current node" that is in the process of being constructed.
        Node<K,V> * currNodeAtDepth[height];
        for (int i=0;i<height;++i) currNodeAtDepth[i] = NULL;
        
        for (size_t keyIx=0;keyIx<initNumKeys;++keyIx) {
            
            // fetch key and value to be inserted next
            const K& key = initKeys[keyIx];
            const V& val = initValues[keyIx];

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
                    uint64_t rint = rng.next();
                    double r = rint / (double) std::numeric_limits<uint64_t>::max();
                    double probOneLarger = keysPerNodeAtDepth[i] - floorKeysPerNodeAtDepth[i];
                    size_t nodeDegree = floorKeysPerNodeAtDepth[i];
                    if (r < probOneLarger) ++nodeDegree;
                    if (i==0) nodeDegree *= 2; // make root bigger so it never overflows (HACK!)
                    if (i==0) assert(currNodeAtDepth[i]==NULL); // non-null root never gets replaced
                    
                    auto curr = createNode(tid, nodeDegree);
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
            auto kvptr = createKVPair(tid, key, val);
            
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
        *root->ptrAddr(0) = NODE_TO_CASWORD(currNodeAtDepth[0]);
        assert(currNodeAtDepth[0]);
        
//        // bfs debug print
//        std::vector<Node<K,V>*> q;
//        q.push_back(root);
//        int k=0;
//        while (k < q.size()) {
//            auto node = q[k++]; // pop
//            if (node == NULL) {
//                std::cout<<"null"<<std::endl;
//                continue;
//            }
//            std::cout<<" degree="<<node->degree;
//            for (int i=0;i<node->degree-1;++i) {
//                std::cout<<(i==0?":":",")<<node->key(i);
//            }
//            std::cout<<std::endl;
//            std::cout<<" ptrs";
//            for (int i=0;i<node->degree;++i) {
//                std::cout<<(i==0?":":",")<<(size_t)node->ptr(i);
//            }
//            std::cout<<std::endl;
//            
//            for (int i=0;i<node->degree;++i) {
//                auto ptr = node->ptr(i);
//                if (IS_KVPAIR(ptr)) {
//                    
//                } else if (IS_REBUILDOP(ptr)) {
//                    
//                } else {
//                    assert(IS_NODE(ptr));
//                    q.push_back(CASWORD_TO_NODE(ptr));
//                }
//            }
//        }
        
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

private:
    /*******************************************************************
     * Utility functions for integration with the test harness
     *******************************************************************/

    int sequentialSize(const casword_t ptr) {
        if (IS_KVPAIR(ptr)) {
            return 1;
        }
        int retval = 0;
        auto node = CASWORD_TO_NODE(ptr);
        for (int i=0;i<node->degree;++i) {
            const casword_t child = node->ptr(i);
            retval += sequentialSize(child);
        }
        return retval;
    }
    int sequentialSize() {
        return sequentialSize(root->ptr(0));
    }

    int getNumberOfLeaves(Node<K,V>* node) {
//        if (node == NULL) return 0;
//        if (node->isLeaf()) return 1;
//        int result = 0;
//        for (int i=0;i<node->getABDegree();++i) {
//            result += getNumberOfLeaves(node->ptrs[i]);
//        }
//        return result;
        return 0;
    }
    const int getNumberOfLeaves() {
//        return getNumberOfLeaves(root->ptrs[0]);
        return 0;
    }
    int getNumberOfInternals(Node<K,V>* node) {
//        if (node == NULL) return 0;
//        if (node->isLeaf()) return 0;
//        int result = 1;
//        for (int i=0;i<node->getABDegree();++i) {
//            result += getNumberOfInternals(node->ptrs[i]);
//        }
//        return result;
        return 0;
    }
    const int getNumberOfInternals() {
        return 0;
//        return getNumberOfInternals(root->ptrs[0]);
    }
    const int getNumberOfNodes() {
        return 0;
//        return getNumberOfLeaves() + getNumberOfInternals();
    }

    int getSumOfKeyDepths(Node<K,V>* node, int depth) {
        return 0;
//        if (node == NULL) return 0;
//        if (node->isLeaf()) return depth * node->getKeyCount();
//        int result = 0;
//        for (int i=0;i<node->getABDegree();i++) {
//            result += getSumOfKeyDepths(node->ptrs[i], 1+depth);
//        }
//        return result;
    }
    const int getSumOfKeyDepths() {
        return 0;
//        return getSumOfKeyDepths(root->ptrs[0], 0);
    }
    const double getAverageKeyDepth() {
        return 0;
//        long sz = sequentialSize();
//        return (sz == 0) ? 0 : getSumOfKeyDepths() / sz;
    }

    int getHeight(Node<K,V>* node, int depth) {
//        if (node == NULL) return 0;
//        if (node->isLeaf()) return 0;
//        int result = 0;
//        for (int i=0;i<node->getABDegree();i++) {
//            int retval = getHeight(node->ptrs[i], 1+depth);
//            if (retval > result) result = retval;
//        }
//        return result+1;
        return 0;
    }
    const int getHeight() {
        return 0;
//        return getHeight(root->ptrs[0], 0);
    }

    int getKeyCount(Node<K,V>* root) {
//        if (root == NULL) return 0;
//        if (root->isLeaf()) return root->getKeyCount();
//        int sum = 0;
//        for (int i=0;i<root->getABDegree();++i) {
//            sum += getKeyCount(root->ptrs[i]);
//        }
//        return sum;
        return 0;
    }
    int getTotalDegree(Node<K,V>* root) {
//        if (root == NULL) return 0;
//        int sum = root->getKeyCount();
//        if (root->isLeaf()) return sum;
//        for (int i=0;i<root->getABDegree();++i) {
//            sum += getTotalDegree(root->ptrs[i]);
//        }
//        return 1+sum; // one more children than keys
        return 0;
    }
    int getNodeCount(Node<K,V>* root) {
//        if (root == NULL) return 0;
//        if (root->isLeaf()) return 1;
//        int sum = 1;
//        for (int i=0;i<root->getABDegree();++i) {
//            sum += getNodeCount(root->ptrs[i]);
//        }
//        return sum;
        return 0;
    }
    double getAverageDegree() {
        return 0;
//        return getTotalDegree(root) / (double) getNodeCount(root);
    }
    double getSpacePerKey() {
        return 0;
//        return getNodeCount(root)*2*b / (double) getKeyCount(root);
    }

    long long getSumOfKeys(const casword_t ptr) {
        if (IS_KVPAIR(ptr)) {
            long long key = (long long) CASWORD_TO_KVPAIR(ptr)->k;
            return (key == INF_KEY) ? 0 : key;
        }
        auto node = CASWORD_TO_NODE(ptr);
        long long sum = 0;
        for (int i=0;i<node->degree;++i) {
            sum += getSumOfKeys(node->ptr(i));
        }
        return sum;
    }
    long long getSumOfKeys() {
        return getSumOfKeys(root->ptr(0));
    }

    void istree_error(std::string s) {
        std::cerr<<"ERROR: "<<s<<std::endl;
        exit(-1);
    }

    void debugPrint() {
        std::cout<<"averageDegree="<<getAverageDegree()<<std::endl;
        std::cout<<"averageDepth="<<getAverageKeyDepth()<<std::endl;
        std::cout<<"height="<<getHeight()<<std::endl;
        std::cout<<"internalNodes="<<getNumberOfInternals()<<std::endl;
        std::cout<<"leafNodes="<<getNumberOfLeaves()<<std::endl;
    }

public:
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
    int rangeQuery(const int tid, const K& low, const K& hi, K * const resultKeys, void ** const resultValues) {
        istree_error("not implemented");
    }
    bool validate(const long long keysum, const bool checkkeysum) {
        if (checkkeysum) {
            long long treekeysum = getSumOfKeys();
            if (treekeysum != keysum) {
                std::cerr<<"ERROR: tree keysum "<<treekeysum<<" did not match thread keysum "<<keysum<<std::endl;
                return false;
            }
        }
        return true;
    }

    long long getSizeInNodes() {
        return getNumberOfNodes();
    }
    std::string getSizeString() {
        std::stringstream ss;
        ss<<getSizeInNodes()<<" nodes in tree";
        return ss.str();
    }
    long long getSize(Node<K,V> * node) {
        return sequentialSize(node);
    }
    long long getSize() {
        return sequentialSize();
    }
    RecManager * const debugGetRecMgr() {
        return recordmgr;
    }
    long long debugKeySum() {
        return getSumOfKeys();
    }
};

#endif	/* ISTREE_H */

