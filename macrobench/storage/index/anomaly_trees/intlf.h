/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   intlf.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 7, 2017, 4:00 PM
 */

#ifndef INTLF_H
#define INTLF_H

#if     (INDEX_STRUCT == IDX_INTLF) 
#define SIMPLE_SEEK
#elif   (INDEX_STRUCT == IDX_INTLF_PAD) 
#define SIMPLE_SEEK
#define PAD 
#define PAD_SIZE 24
#elif   (INDEX_STRUCT == IDX_INTLF_BASELINE)
#define BASELINE
#else
#error
#endif

#define K 2
#define LEFT 0
#define RIGHT 1

//#define MAX_KEY 0x7FFFFFFFFFFFFFFF
#define INF_R 0x7FFFFFFFFFFFFFFF
#define INF_S 0x7FFFFFFFFFFFFFFF
#define INF_T 0x7FFFFFFFFFFFFFFF 
#define KEY_MASK  0x8000000000000000//0x80000000 //are these mask correct???  the orginal was correct only for 32bit keys (max 4 byte keys)
#define ADDRESS_MASK 15 

#define NULL_BIT 8
#define INJECT_BIT 4
#define DELETE_BIT 2
#define PROMOTE_BIT 1

typedef enum {
    INJECTION, DISCOVERY, CLEANUP
} Mode;

typedef enum {
    SIMPLE, COMPLEX
} Type;

typedef enum {
    DELETE_FLAG, PROMOTE_FLAG
} Flag;

template <typename skey_t, typename sval_t>
struct node_t {
    volatile skey_t markAndKey; //format <markFlag,address>
    struct node_t<skey_t, sval_t> * volatile child[K]; //format <address,NullBit,InjectFlag,DeleteFlag,PromoteFlag>
    volatile unsigned long readyToReplace;

    sval_t value;

#ifdef PAD
    char pad[PAD_SIZE];
#endif
};

template <typename skey_t, typename sval_t>
struct edge {
    struct node_t<skey_t, sval_t>* parent;
    struct node_t<skey_t, sval_t>* child;
    int which;
};

template <typename skey_t, typename sval_t>
struct seekRecord {
    struct edge<skey_t, sval_t> lastEdge;
    struct edge<skey_t, sval_t> pLastEdge;
    struct edge<skey_t, sval_t> injectionEdge;
};

template <typename skey_t, typename sval_t>
struct anchorRecord {
    struct node_t<skey_t, sval_t>* node;
    skey_t key;
};

template <typename skey_t, typename sval_t>
struct stateRecord {
    int depth;
    struct edge<skey_t, sval_t> targetEdge;
    struct edge<skey_t, sval_t> pTargetEdge;
    skey_t targetKey;
    skey_t currentKey;
    Mode mode;
    Type type;
    struct seekRecord<skey_t, sval_t> successorRecord;
};

template <typename skey_t, typename sval_t>
struct tArgs {
    int tId;
    unsigned long lseed;
    unsigned long readCount;
    unsigned long successfulReads;
    unsigned long unsuccessfulReads;
    unsigned long readRetries;
    unsigned long insertCount;
    unsigned long successfulInserts;
    unsigned long unsuccessfulInserts;
    unsigned long insertRetries;
    unsigned long deleteCount;
    unsigned long successfulDeletes;
    unsigned long unsuccessfulDeletes;
    unsigned long deleteRetries;
    struct node_t<skey_t, sval_t>* newNode;
    bool isNewNodeAvailable;
    struct seekRecord<skey_t, sval_t> targetRecord;
    struct seekRecord<skey_t, sval_t> pSeekRecord;
    struct stateRecord<skey_t, sval_t> myState;
    struct anchorRecord<skey_t, sval_t> anchorRecord;
    struct anchorRecord<skey_t, sval_t> pAnchorRecord;
    unsigned long seekRetries;
    unsigned long seekLength;
};

template <typename skey_t, typename sval_t, class RecMgr>
class intlf {
private:
    struct node_t<skey_t, sval_t>* R;
    struct node_t<skey_t, sval_t>* S;
    struct node_t<skey_t, sval_t>* T;
    unsigned long numOfNodes;

    void populateEdge(struct edge<skey_t, sval_t>* e, struct node_t<skey_t, sval_t>* parent, struct node_t<skey_t, sval_t>* child, int which);
    void copyEdge(struct edge<skey_t, sval_t>* d, struct edge<skey_t, sval_t>* s);
    void copySeekRecord(struct seekRecord<skey_t, sval_t>* d, struct seekRecord<skey_t, sval_t>* s);
    struct node_t<skey_t, sval_t>* newLeafNode(skey_t key, sval_t value);
    void seek(struct tArgs<skey_t, sval_t>*, skey_t, struct seekRecord<skey_t, sval_t>*);
    void initializeTypeAndUpdateMode(struct tArgs<skey_t, sval_t>*, struct stateRecord<skey_t, sval_t>*);
    void updateMode(struct tArgs<skey_t, sval_t>*, struct stateRecord<skey_t, sval_t>*);
    void inject(struct tArgs<skey_t, sval_t>*, struct stateRecord<skey_t, sval_t>*);
    void findSmallest(struct tArgs<skey_t, sval_t>*, struct node_t<skey_t, sval_t>*, struct seekRecord<skey_t, sval_t>*);
    void findAndMarkSuccessor(struct tArgs<skey_t, sval_t>*, struct stateRecord<skey_t, sval_t>*);
    void removeSuccessor(struct tArgs<skey_t, sval_t>*, struct stateRecord<skey_t, sval_t>*);
    bool cleanup(struct tArgs<skey_t, sval_t>*, struct stateRecord<skey_t, sval_t>*);
    bool markChildEdge(struct tArgs<skey_t, sval_t>*, struct stateRecord<skey_t, sval_t>*, bool);
    void helpTargetNode(struct tArgs<skey_t, sval_t>*, struct edge<skey_t, sval_t>*, int);
    void helpSuccessorNode(struct tArgs<skey_t, sval_t>*, struct edge<skey_t, sval_t>*, int);
    struct node_t<skey_t, sval_t>* simpleSeek(skey_t key, struct seekRecord<skey_t, sval_t>* s);
    sval_t search(struct tArgs<skey_t, sval_t>* t, skey_t key);
    sval_t lf_insert(struct tArgs<skey_t, sval_t>* t, skey_t key, sval_t value);
    bool lf_remove(struct tArgs<skey_t, sval_t>* t, skey_t key);
    
public:

    intlf() {
        R = newLeafNode(INF_R,NULL);
        R->child[RIGHT] = newLeafNode(INF_S,NULL);
        S = R->child[RIGHT];
	S->child[RIGHT] = newLeafNode(INF_T,NULL);
	T = S->child[RIGHT];
    }

    ~intlf() {
    }

    void initThread(const int tid) {
    }

    void deinitThread(const int tid) {
    }

    sval_t insert(skey_t key, sval_t item) {
        assert(key < INF_T); // is this reasonable.... not for hashed keys
        struct tArgs<skey_t, sval_t> args = {0}; 
        return lf_insert(&args, key, item);
    }

    sval_t find(skey_t key) {
        struct tArgs<skey_t, sval_t> args = {0}; 
        return search(&args, key);
    }

    node_t<skey_t, sval_t> * get_root() {
        return R;
    }

    node_t<skey_t, sval_t> * get_left(node_t<skey_t, sval_t> * curr);
    node_t<skey_t, sval_t> * get_right(node_t<skey_t, sval_t> * curr);
};
#endif /* INTLF_H */

