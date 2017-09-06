/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   intlf_impl.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 7, 2017, 4:00 PM
 */

#ifndef INTLF_IMPL_H
#define INTLF_IMPL_H

#include "intlf.h"

static __inline bool isIFlagSet(struct node_t<skey_t, sval_t>* p) {
    return ((uintptr_t) p & INJECT_BIT) != 0;
}

static __inline bool isNull(struct node_t<skey_t, sval_t>* p) {
    return ((uintptr_t) p & NULL_BIT) != 0;
}

static __inline bool isDFlagSet(struct node_t<skey_t, sval_t>* p) {
    return ((uintptr_t) p & DELETE_BIT) != 0;
}

static __inline bool isPFlagSet(struct node_t<skey_t, sval_t>* p) {
    return ((uintptr_t) p & PROMOTE_BIT) != 0;
}

static __inline bool isKeyMarked(skey_t key) {
    return ((key & KEY_MASK) == KEY_MASK);
}

static __inline struct node_t<skey_t, sval_t>* setIFlag(struct node_t<skey_t, sval_t>* p) {
    return (struct node_t<skey_t, sval_t>*) ((uintptr_t) p | INJECT_BIT);
}

static __inline struct node_t<skey_t, sval_t>* setNull(struct node_t<skey_t, sval_t>* p) {
    return (struct node_t<skey_t, sval_t>*) ((uintptr_t) p | NULL_BIT);
}

static __inline struct node_t<skey_t, sval_t>* setDFlag(struct node_t<skey_t, sval_t>* p) {
    return (struct node_t<skey_t, sval_t>*) ((uintptr_t) p | DELETE_BIT);
}

static __inline struct node_t<skey_t, sval_t>* setPFlag(struct node_t<skey_t, sval_t>* p) {
    return (struct node_t<skey_t, sval_t>*) ((uintptr_t) p | PROMOTE_BIT);
}

static __inline skey_t setReplaceFlagInKey(skey_t key) {
    return (key | KEY_MASK);
}

__inline skey_t getKey(skey_t key) {
    return (key & MAX_KEY);
}

static __inline struct node_t<skey_t, sval_t>* getAddress(struct node_t<skey_t, sval_t>* p) {
    return (struct node_t<skey_t, sval_t>*)((uintptr_t) p & ~ADDRESS_MASK);
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * intlf<skey_t, sval_t, RecMgr>::get_left(node_t<skey_t, sval_t> * curr) {
    if (isNull(getAddress(curr)->child[0])) {
        return NULL;
    }
    return getAddress(getAddress(curr)->child[0]);
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t> * intlf<skey_t, sval_t, RecMgr>::get_right(node_t<skey_t, sval_t> * curr) {
    if (isNull(getAddress(curr)->child[1])) {
        return NULL;
    }
    return getAddress(getAddress(curr)->child[1]);
}

static inline bool CAS(struct node_t<skey_t, sval_t>* parent, int which, struct node_t<skey_t, sval_t>* oldChild, struct node_t<skey_t, sval_t>* newChild) {
    return __sync_bool_compare_and_swap(&parent->child[which], oldChild, newChild);
}

template <typename skey_t, typename sval_t, class RecMgr>
inline struct node_t<skey_t, sval_t>* intlf<skey_t, sval_t, RecMgr>::newLeafNode(skey_t key, sval_t value) {
    struct node_t<skey_t, sval_t>* node = (struct node_t<skey_t, sval_t>*) tree_malloc::allocnode(sizeof (struct node_t<skey_t, sval_t>));
    node->markAndKey = key;
    node->value = value;
    node->child[LEFT] = setNull(NULL);
    node->child[RIGHT] = setNull(NULL);
    node->readyToReplace = false;
    return node;
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::populateEdge(struct edge<skey_t, sval_t>* e, struct node_t<skey_t, sval_t>* parent, struct node_t<skey_t, sval_t>* child, int which) {
    e->parent = parent;
    e->child = child;
    e->which = which;
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::copyEdge(struct edge<skey_t, sval_t>* d, struct edge<skey_t, sval_t>* s) {
    d->parent = s->parent;
    d->child = s->child;
    d->which = s->which;
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::copySeekRecord(struct seekRecord<skey_t, sval_t>* d, struct seekRecord<skey_t, sval_t>* s) {
    copyEdge(&d->lastEdge, &s->lastEdge);
    copyEdge(&d->pLastEdge, &s->pLastEdge);
    copyEdge(&d->injectionEdge, &s->injectionEdge);
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::seek(struct tArgs<skey_t, sval_t>* t, skey_t key, struct seekRecord<skey_t, sval_t>* s) {
    struct anchorRecord<skey_t, sval_t>* pAnchorRecord;
    struct anchorRecord<skey_t, sval_t>* anchorRecord;

    struct edge<skey_t, sval_t> pLastEdge;
    struct edge<skey_t, sval_t> lastEdge;

    struct node_t<skey_t, sval_t>* curr;
    struct node_t<skey_t, sval_t>* next;
    struct node_t<skey_t, sval_t>* temp;
    int which;

    bool n;
    bool d;
    bool p;
    skey_t cKey;
    skey_t aKey;

    pAnchorRecord = &t->pAnchorRecord;
    anchorRecord = &t->anchorRecord;

    pAnchorRecord->node = S;
    pAnchorRecord->key = INF_S;

    while (true) {
        //initialize all variables used in traversal
        populateEdge(&pLastEdge, R, S, RIGHT);
        populateEdge(&lastEdge, S, T, RIGHT);
        curr = T;
        anchorRecord->node = S;
        anchorRecord->key = INF_S;
        while (true) {
            t->seekLength++;
            //read the key stored in the current node
            cKey = getKey(curr->markAndKey);
            //find the next edge to follow
            which = key < cKey ? LEFT : RIGHT;
            temp = curr->child[which];
            n = isNull(temp);
            d = isDFlagSet(temp);
            p = isPFlagSet(temp);
            next = getAddress(temp);
            //check for completion of the traversal
            if (key == cKey || n) {
                //either key found or no next edge to follow. Stop the traversal
                s->pLastEdge = pLastEdge;
                s->lastEdge = lastEdge;
                populateEdge(&s->injectionEdge, curr, next, which);
                if (key == cKey) {
                    //key matches. So return
                    return;
                } else {
                    break;
                }
            }
            if (which == RIGHT) {
                //the next edge that will be traversed is a right edge. Keep track of the current node and its key
                anchorRecord->node = curr;
                anchorRecord->key = cKey;
            }
            //traverse the next edge
            pLastEdge = lastEdge;
            populateEdge(&lastEdge, curr, next, which);
            curr = next;
        }
        //key was not found. check if can stop
        aKey = getKey(anchorRecord->node->markAndKey);
        if (anchorRecord->key == aKey) {
            temp = anchorRecord->node->child[RIGHT];
            d = isDFlagSet(temp);
            p = isPFlagSet(temp);
            if (!d && !p) {
                //the anchor node is part of the tree. Return the results of the current traversal
                return;
            }
            if (pAnchorRecord->node == anchorRecord->node && pAnchorRecord->key == anchorRecord->key) {
                //return the results of previous traversal
                copySeekRecord(s, &t->pSeekRecord);
                return;
            }
        }
        //store the results of the current traversal and restart
        copySeekRecord(&t->pSeekRecord, s);
        pAnchorRecord->node = anchorRecord->node;
        pAnchorRecord->key = anchorRecord->key;
        t->seekRetries++;
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::initializeTypeAndUpdateMode(struct tArgs<skey_t, sval_t>* t, struct stateRecord<skey_t, sval_t>* state) {
    struct node_t<skey_t, sval_t>* node;
    //retrieve the address from the state record
    node = state->targetEdge.child;
    if (isNull(node->child[LEFT]) || isNull(node->child[RIGHT])) {
        //one of the child pointers is null
        if (isKeyMarked(node->markAndKey)) {
            state->type = COMPLEX;
        } else {
            state->type = SIMPLE;
        }
    } else {
        //both the child pointers are non-null
        state->type = COMPLEX;
    }
    updateMode(t, state);
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::updateMode(struct tArgs<skey_t, sval_t>* t, struct stateRecord<skey_t, sval_t>* state) {
    struct node_t<skey_t, sval_t>* node;
    //retrieve the address from the state record
    node = state->targetEdge.child;

    //update the operation mode
    if (state->type == SIMPLE) {
        //simple delete
        state->mode = CLEANUP;
    } else {
        //complex delete
        if (node->readyToReplace) {
            assert(isKeyMarked(node->markAndKey));
            state->mode = CLEANUP;
        } else {
            state->mode = DISCOVERY;
        }
    }
    return;
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::inject(struct tArgs<skey_t, sval_t>* t, struct stateRecord<skey_t, sval_t>* state) {
    struct node_t<skey_t, sval_t>* parent;
    struct node_t<skey_t, sval_t>* node;
    struct edge<skey_t, sval_t> targetEdge;
    int which;
    bool result;
    bool i;
    bool d;
    bool p;
    struct node_t<skey_t, sval_t>* temp;

    targetEdge = state->targetEdge;
    parent = targetEdge.parent;
    node = targetEdge.child;
    which = targetEdge.which;

    result = CAS(parent, which, node, setIFlag(node));
    if (!result) {
        //unable to set the intention flag on the edge. help if needed
        temp = parent->child[which];
        i = isIFlagSet(temp);
        d = isDFlagSet(temp);
        p = isPFlagSet(temp);

        if (i) {
            helpTargetNode(t, &targetEdge, 1);
        } else if (d) {
            helpTargetNode(t, &state->pTargetEdge, 1);
        } else if (p) {
            helpSuccessorNode(t, &state->pTargetEdge, 1);
        }
        return;
    }
    //mark the left edge for deletion
    result = markChildEdge(t, state, LEFT);
    if (!result) {
        return;
    }
    //mark the right edge for deletion
    result = markChildEdge(t, state, RIGHT);

    //initialize the type and mode of the operation
    initializeTypeAndUpdateMode(t, state);
    return;
}

template <typename skey_t, typename sval_t, class RecMgr>
bool intlf<skey_t, sval_t, RecMgr>::markChildEdge(struct tArgs<skey_t, sval_t>* t, struct stateRecord<skey_t, sval_t>* state, bool which) {
    struct node_t<skey_t, sval_t>* node;
    struct edge<skey_t, sval_t> edge;
    Flag flag;
    struct node_t<skey_t, sval_t>* address;
    struct node_t<skey_t, sval_t>* temp;
    bool n;
    bool i;
    bool d;
    bool p;
    struct edge<skey_t, sval_t> helpeeEdge;
    struct node_t<skey_t, sval_t>* oldValue;
    struct node_t<skey_t, sval_t>* newValue;
    bool result;

    if (state->mode == INJECTION) {
        edge = state->targetEdge;
        flag = DELETE_FLAG;
    } else {
        edge = state->successorRecord.lastEdge;
        flag = PROMOTE_FLAG;
    }
    node = edge.child;
    while (true) {
        temp = node->child[which];
        n = isNull(temp);
        i = isIFlagSet(temp);
        d = isDFlagSet(temp);
        p = isPFlagSet(temp);
        address = getAddress(temp);
        if (i) {
            populateEdge(&helpeeEdge, node, address, which);
            helpTargetNode(t, &helpeeEdge, state->depth + 1);
            continue;
        } else if (d) {
            if (flag == PROMOTE_FLAG) {
                helpTargetNode(t, &edge, state->depth + 1);
                return false;
            } else {
                return true;
            }
        } else if (p) {
            if (flag == DELETE_FLAG) {
                helpSuccessorNode(t, &edge, state->depth + 1);
                return false;
            } else {
                return true;
            }
        }
        if (n) {
            oldValue = setNull(address);
        } else {
            oldValue = address;
        }
        if (flag == DELETE_FLAG) {
            newValue = setDFlag(oldValue);
        } else {
            newValue = setPFlag(oldValue);
        }
        result = CAS(node, which, oldValue, newValue);
        if (!result) {
            continue;
        } else {
            break;
        }
    }
    return true;
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::findSmallest(struct tArgs<skey_t, sval_t>* t, struct node_t<skey_t, sval_t>* node, struct seekRecord<skey_t, sval_t>* s) {
    struct node_t<skey_t, sval_t>* curr;
    struct node_t<skey_t, sval_t>* left;
    struct node_t<skey_t, sval_t>* right;
    struct node_t<skey_t, sval_t>* temp;
    bool n;
    struct edge<skey_t, sval_t> lastEdge;
    struct edge<skey_t, sval_t> pLastEdge;

    //find the node with the smallest key in the subtree rooted at the right child
    //initialize the variables used in the traversal
    right = getAddress(node->child[RIGHT]);
    populateEdge(&lastEdge, node, right, RIGHT);
    populateEdge(&pLastEdge, node, right, RIGHT);
    while (true) {
        curr = lastEdge.child;
        temp = curr->child[LEFT];
        n = isNull(temp);
        left = getAddress(temp);
        if (n) {
            break;
        }
        //traverse the next edge
        pLastEdge = lastEdge;
        populateEdge(&lastEdge, curr, left, LEFT);
    }
    //initialize seek record and return
    s->lastEdge = lastEdge;
    s->pLastEdge = pLastEdge;
    return;
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::findAndMarkSuccessor(struct tArgs<skey_t, sval_t>* t, struct stateRecord<skey_t, sval_t>* state) {
    struct node_t<skey_t, sval_t>* node;
    struct seekRecord<skey_t, sval_t>* s;
    struct edge<skey_t, sval_t> successorEdge;
    bool m;
    bool n;
    bool d;
    bool p;
    bool result;
    struct node_t<skey_t, sval_t>* temp;
    struct node_t<skey_t, sval_t>* left;

    //retrieve the addresses from the state record
    node = state->targetEdge.child;
    s = &state->successorRecord;
    while (true) {
        //read the mark flag of the key in the target node
        m = isKeyMarked(node->markAndKey);
        //find the node with the smallest key in the right subtree
        findSmallest(t, node, s);
        if (m) {
            //successor node has already been selected before the traversal
            break;
        }
        //retrieve the information from the seek record
        successorEdge = s->lastEdge;
        temp = successorEdge.child->child[LEFT];
        n = isNull(temp);
        p = isPFlagSet(temp);
        left = getAddress(temp);
        if (!n) {
            continue;
        }
        //read the mark flag of the key under deletion
        m = isKeyMarked(node->markAndKey);
        if (m) {
            //successor node has already been selected
            if (p) {
                break;
            } else {
                continue;
            }
        }
        //try to set the promote flag on the left edge
        result = CAS(successorEdge.child, LEFT, setNull(left), setPFlag(setNull(node)));
        if (result) {
            break;
        }
        //attempt to mark the edge failed; recover from the failure and retry if needed
        temp = successorEdge.child->child[LEFT];
        n = isNull(temp);
        d = isDFlagSet(temp);
        p = isPFlagSet(temp);
        if (p) {
            break;
        }
        if (!n) {
            //the node found has since gained a left child
            continue;
        }
        if (d) {
            //the node found is undergoing deletion; need to help
            helpTargetNode(t, &s->lastEdge, state->depth + 1);
        }
    }
    // update the operation mode
    updateMode(t, state);
    return;
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::removeSuccessor(struct tArgs<skey_t, sval_t>* t, struct stateRecord<skey_t, sval_t>* state) {
    struct node_t<skey_t, sval_t>* node;
    struct seekRecord<skey_t, sval_t>* s;
    struct edge<skey_t, sval_t> successorEdge;
    struct edge<skey_t, sval_t> pLastEdge;
    struct node_t<skey_t, sval_t>* temp;
    struct node_t<skey_t, sval_t>* right;
    struct node_t<skey_t, sval_t>* address;
    struct node_t<skey_t, sval_t>* oldValue;
    struct node_t<skey_t, sval_t>* newValue;
    bool n;
    bool d;
    bool p;
    bool i;
    bool dFlag;
    bool which;
    bool result;

    //retrieve addresses from the state record
    node = state->targetEdge.child;
    s = &state->successorRecord;
    findSmallest(t, node, s);
    //extract information about the successor node
    successorEdge = s->lastEdge;
    //ascertain that the seek record for the successor node contains valid information
    temp = successorEdge.child->child[LEFT];
    p = isPFlagSet(temp);
    address = getAddress(temp);
    if (address != node) {
        node->readyToReplace = true;
        updateMode(t, state);
        return;
    }
    if (!p) {
        node->readyToReplace = true;
        updateMode(t, state);
        return;
    }

    //mark the right edge for promotion if unmarked
    temp = successorEdge.child->child[RIGHT];
    p = isPFlagSet(temp);
    if (!p) {
        //set the promote flag on the right edge
        markChildEdge(t, state, RIGHT);
    }
    //promote the key
    node->markAndKey = setReplaceFlagInKey(successorEdge.child->markAndKey);
    while (true) {
        //check if the successor is the right child of the target node itself
        if (successorEdge.parent == node) {
            dFlag = true;
            which = RIGHT;
        } else {
            dFlag = false;
            which = LEFT;
        }
        i = isIFlagSet(successorEdge.parent->child[which]);
        temp = successorEdge.child->child[RIGHT];
        n = isNull(temp);
        right = getAddress(temp);
        if (n) {
            //only set the null flag. do not change the address
            if (i) {
                if (dFlag) {
                    oldValue = setIFlag(setDFlag(successorEdge.child));
                    newValue = setNull(setDFlag(successorEdge.child));
                } else {
                    oldValue = setIFlag(successorEdge.child);
                    newValue = setNull(successorEdge.child);
                }
            } else {
                if (dFlag) {
                    oldValue = setDFlag(successorEdge.child);
                    newValue = setNull(setDFlag(successorEdge.child));
                } else {
                    oldValue = successorEdge.child;
                    newValue = setNull(successorEdge.child);
                }
            }
            result = CAS(successorEdge.parent, which, oldValue, newValue);
        } else {
            if (i) {
                if (dFlag) {
                    oldValue = setIFlag(setDFlag(successorEdge.child));
                    newValue = setDFlag(right);
                } else {
                    oldValue = setIFlag(successorEdge.child);
                    newValue = right;
                }
            } else {
                if (dFlag) {
                    oldValue = setDFlag(successorEdge.child);
                    newValue = setDFlag(right);
                } else {
                    oldValue = successorEdge.child;
                    newValue = right;
                }
            }
            result = CAS(successorEdge.parent, which, oldValue, newValue);
        }
        if (result) {
            break;
        }
        if (dFlag) {
            break;
        }
        temp = successorEdge.parent->child[which];
        d = isDFlagSet(temp);
        pLastEdge = s->pLastEdge;
        if (d && pLastEdge.parent != NULL) {
            helpTargetNode(t, &pLastEdge, state->depth + 1);
        }
        findSmallest(t, node, s);
        if (s->lastEdge.child != successorEdge.child) {
            //the successor node has already been removed
            break;
        } else {
            successorEdge = s->lastEdge;
        }
    }
    node->readyToReplace = true;
    updateMode(t, state);
    return;
}

template <typename skey_t, typename sval_t, class RecMgr>
bool intlf<skey_t, sval_t, RecMgr>::cleanup(struct tArgs<skey_t, sval_t>* t, struct stateRecord<skey_t, sval_t>* state) {
    struct node_t<skey_t, sval_t>* parent;
    struct node_t<skey_t, sval_t>* node;
    struct node_t<skey_t, sval_t>* newNode;
    struct node_t<skey_t, sval_t>* left;
    struct node_t<skey_t, sval_t>* right;
    struct node_t<skey_t, sval_t>* address;
    struct node_t<skey_t, sval_t>* temp;
    bool pWhich;
    bool nWhich;
    bool result;
    bool n;

    //retrieve the addresses from the state record
    parent = state->targetEdge.parent;
    node = state->targetEdge.child;
    pWhich = state->targetEdge.which;

    if (state->type == COMPLEX) {
        //replace the node with a new copy in which all the fields are unmarked
        newNode = (struct node_t<skey_t, sval_t>*) tree_malloc::allocnode(sizeof (struct node_t<skey_t, sval_t>));
        newNode->markAndKey = getKey(node->markAndKey);
        newNode->value = 0;
        newNode->readyToReplace = false;
        left = getAddress(node->child[LEFT]);
        newNode->child[LEFT] = left;
        temp = node->child[RIGHT];
        n = isNull(temp);
        right = getAddress(temp);
        if (n) {
            newNode->child[RIGHT] = setNull(NULL);
        } else {
            newNode->child[RIGHT] = right;
        }

        //switch the edge at the parent
        result = CAS(parent, pWhich, setIFlag(node), newNode);
    } else {
        //remove the node. determine to which grand child will the edge at the parent be switched
        if (isNull(node->child[LEFT])) {
            nWhich = RIGHT;
        } else {
            nWhich = LEFT;
        }
        temp = node->child[nWhich];
        n = isNull(temp);
        address = getAddress(temp);
        if (n) {
            //set the null flag only; do not change the address
            result = CAS(parent, pWhich, setIFlag(node), setNull(node));
        } else {
            result = CAS(parent, pWhich, setIFlag(node), address);
        }
    }
    return result;
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::helpTargetNode(struct tArgs<skey_t, sval_t>* t, struct edge<skey_t, sval_t>* helpeeEdge, int depth) {
    struct stateRecord<skey_t, sval_t>* state;
    bool result;
    // intention flag must be set on the edge
    // obtain new state record and initialize it
    state = (struct stateRecord<skey_t, sval_t>*) tree_malloc::alloc(sizeof (struct stateRecord<skey_t, sval_t>));
    state->targetEdge = *helpeeEdge;
    state->depth = depth;
    state->mode = INJECTION;

    // mark the left and right edges if unmarked
    result = markChildEdge(t, state, LEFT);
    if (!result) {
        return;
    }
    markChildEdge(t, state, RIGHT);
    initializeTypeAndUpdateMode(t, state);
    if (state->mode == DISCOVERY) {
        findAndMarkSuccessor(t, state);
    }

    if (state->mode == DISCOVERY) {
        removeSuccessor(t, state);
    }
    if (state->mode == CLEANUP) {
        cleanup(t, state);
    }

    return;
}

template <typename skey_t, typename sval_t, class RecMgr>
void intlf<skey_t, sval_t, RecMgr>::helpSuccessorNode(struct tArgs<skey_t, sval_t>* t, struct edge<skey_t, sval_t>* helpeeEdge, int depth) {
    struct node_t<skey_t, sval_t>* parent;
    struct node_t<skey_t, sval_t>* node;
    struct node_t<skey_t, sval_t>* left;
    struct stateRecord<skey_t, sval_t>* state;
    struct seekRecord<skey_t, sval_t>* s;
    // retrieve the address of the successor node
    parent = helpeeEdge->parent;
    node = helpeeEdge->child;
    // promote flag must be set on the successor node's left edge
    // retrieve the address of the target node
    left = getAddress(node->child[LEFT]);
    // obtain new state record and initialize it
    state = (struct stateRecord<skey_t, sval_t>*) tree_malloc::alloc(sizeof (struct stateRecord<skey_t, sval_t>));
    populateEdge(&state->targetEdge, NULL, left, LEFT);
    state->depth = depth;
    state->mode = DISCOVERY;
    s = &state->successorRecord;
    // initialize the seek record in the state record
    s->lastEdge = *helpeeEdge;
    populateEdge(&s->pLastEdge, NULL, parent, LEFT);
    // remove the successor node
    removeSuccessor(t, state);
    return;
}

template <typename skey_t, typename sval_t, class RecMgr>
struct node_t<skey_t, sval_t>* intlf<skey_t, sval_t, RecMgr>::simpleSeek(skey_t key, struct seekRecord<skey_t, sval_t>* s) {
    struct anchorRecord<skey_t, sval_t> pAnchorRecord;
    struct anchorRecord<skey_t, sval_t> anchorRecord;

    //struct edge pLastEdge;
    //struct edge lastEdge;

    struct node_t<skey_t, sval_t>* lastTraversalResult = NULL;
    struct node_t<skey_t, sval_t>* curr;
    struct node_t<skey_t, sval_t>* next;
    struct node_t<skey_t, sval_t>* temp;
    int which;

    bool n;
    bool d;
    bool p;
    skey_t cKey;
    skey_t aKey;

    pAnchorRecord.node = S;
    pAnchorRecord.key = INF_S;

    while (true) {
        //initialize all variables used in traversal
        //populateEdge(&pLastEdge,R,S,RIGHT);
        //populateEdge(&lastEdge,S,T,RIGHT);
        curr = T;
        anchorRecord.node = S;
        anchorRecord.key = INF_S;
        while (true) {
            //read the key stored in the current node
            cKey = getKey(curr->markAndKey);
            //find the next edge to follow
            which = key < cKey ? LEFT : RIGHT;
            temp = curr->child[which];
            n = isNull(temp);
            d = isDFlagSet(temp);
            p = isPFlagSet(temp);
            next = getAddress(temp);
            //check for completion of the traversal
            if (key == cKey || n) {
                //either key found or no next edge to follow. Stop the traversal
                //s->pLastEdge = pLastEdge;
                //s->lastEdge = lastEdge;
                //populateEdge(&s->injectionEdge,curr,next,which);
                if (key == cKey) {
                    //key matches. So return
                    return curr;
                } else {
                    break;
                }
            }
            if (which == RIGHT) {
                //the next edge that will be traversed is a right edge. Keep track of the current node and its key
                anchorRecord.node = curr;
                anchorRecord.key = cKey;
            }
            //traverse the next edge
            //pLastEdge = lastEdge;
            //populateEdge(&lastEdge,curr,next,which);
            curr = next;
        }
        //key was not found. check if can stop
        aKey = getKey(anchorRecord.node->markAndKey);
        if (anchorRecord.key == aKey) {
            temp = anchorRecord.node->child[RIGHT];
            d = isDFlagSet(temp);
            p = isPFlagSet(temp);
            if (!d && !p) {
                //the anchor node is part of the tree. Return the results of the current traversal
                return NULL;
            }
            if (pAnchorRecord.node == anchorRecord.node && pAnchorRecord.key == anchorRecord.key) {
                //return the results of previous traversal
                return lastTraversalResult;
            }
        }
        //store the results of the current traversal and restart
        lastTraversalResult = curr;
        pAnchorRecord.node = anchorRecord.node;
        pAnchorRecord.key = anchorRecord.key;
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t intlf<skey_t, sval_t, RecMgr>::search(struct tArgs<skey_t, sval_t>* t, skey_t key) {
    struct node_t<skey_t, sval_t>* node;

#ifndef SIMPLE_SEEK
    seek(t, key, &t->targetRecord);
    node = t->targetRecord.lastEdge.child;
    skey_t nKey = getKey(node->markAndKey);
    if (nKey == key) {
        t->successfulReads++;
        return node->value;
    } 
    t->unsuccessfulReads++;
    return NULL;
    
#else
    node = simpleSeek(key, &t->targetRecord);
    if (node == NULL) {
        return (NULL);
    } else {
        return (node->value);
    }
#endif
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t intlf<skey_t, sval_t, RecMgr>::lf_insert(struct tArgs<skey_t, sval_t>* t, skey_t key, sval_t value) {
    struct node_t<skey_t, sval_t>* node;
    struct node_t<skey_t, sval_t>* newNode;
    struct node_t<skey_t, sval_t>* address;
    skey_t nKey;
    bool result;
    int which;
    struct node_t<skey_t, sval_t>* temp;

    while (true) {
        seek(t, key, &t->targetRecord);
        node = t->targetRecord.lastEdge.child;
        nKey = getKey(node->markAndKey);
        if (nKey == key) {
            t->unsuccessfulInserts++;
            return node->value;
        }

        //create a new node and initialize its fields
        if (!t->isNewNodeAvailable) {
            t->newNode = newLeafNode(key, value);
            t->isNewNodeAvailable = true;
        }
        newNode = t->newNode;
        newNode->markAndKey = key;
        newNode->value = value;
        which = t->targetRecord.injectionEdge.which;
        address = t->targetRecord.injectionEdge.child;
        result = CAS(node, which, setNull(address), newNode);
        if (result) {
            t->isNewNodeAvailable = false;
            t->successfulInserts++;
            return NULL;
        }
        t->insertRetries++;

        temp = node->child[which];
        if (isDFlagSet(temp)) {
            helpTargetNode(t, &t->targetRecord.lastEdge, 1);
        } else if (isPFlagSet(temp)) {
            helpSuccessorNode(t, &t->targetRecord.lastEdge, 1);
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
bool intlf<skey_t, sval_t, RecMgr>::lf_remove(struct tArgs<skey_t, sval_t>* t, skey_t key) {
    struct stateRecord<skey_t, sval_t>* myState;
    struct seekRecord<skey_t, sval_t>* targetRecord;
    struct edge<skey_t, sval_t> targetEdge;
    struct edge<skey_t, sval_t> pTargetEdge;
    skey_t nKey;
    bool result;

    //initialize the state record
    myState = &t->myState;
    myState->depth = 0;
    myState->targetKey = key;
    myState->currentKey = key;
    myState->mode = INJECTION;
    targetRecord = &t->targetRecord;

    while (true) {
        seek(t, myState->currentKey, targetRecord);
        targetEdge = targetRecord->lastEdge;
        pTargetEdge = targetRecord->pLastEdge;
        nKey = getKey(targetEdge.child->markAndKey);
        if (myState->currentKey != nKey) {
            //the key does not exist in the tree
            if (myState->mode == INJECTION) {
                t->unsuccessfulDeletes++;
                return (false);
            } else {
                t->successfulDeletes++;
                return (true);
            }
        }
        //perform appropriate action depending on the mode
        if (myState->mode == INJECTION) {
            //store a reference to the target node
            myState->targetEdge = targetEdge;
            myState->pTargetEdge = pTargetEdge;
            //attempt to inject the operation at the node
            inject(t, myState);
        }
        //mode would have changed if the operation was injected successfully
        if (myState->mode != INJECTION) {
            //if the target node found by the seek function is different from the one stored in state record, then return
            if (myState->targetEdge.child != targetEdge.child) {
                t->successfulDeletes++;
                return (true);
            }
            //update the target edge using the most recent seek
            myState->targetEdge = targetEdge;
        }
        if (myState->mode == DISCOVERY) {
            findAndMarkSuccessor(t, myState);
        }
        if (myState->mode == DISCOVERY) {
            removeSuccessor(t, myState);
        }
        if (myState->mode == CLEANUP) {
            result = cleanup(t, myState);
            if (result) {
                t->successfulDeletes++;
                return (true);
            } else {
                nKey = getKey(targetEdge.child->markAndKey);
                myState->currentKey = nKey;
            }
        }
    }
}


#endif /* INTLF_IMPL_H */

