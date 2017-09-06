/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   citrus_impl.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 10, 2017, 11:45 AM
 */

#ifndef CITRUS_IMP_H
#define CITRUS_IMP_H

#include "citrus.h"

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* citrus<skey_t, sval_t, RecMgr>::newNode(skey_t key, sval_t value) {
    node_t<skey_t, sval_t> * nnode = (node_t<skey_t, sval_t>*) tree_malloc::allocnode(sizeof (struct node_t<skey_t, sval_t>));
    nnode->key = key;
    nnode->marked = false;
    nnode->child[0] = NULL;
    nnode->child[1] = NULL;
    nnode->tag[0] = 0;
    nnode->tag[1] = 0;
    nnode->value = value;
    if (mutex_init(&(nnode->lock)) != 0) {
        printf("\n mutex init failed\n");
    }
    return nnode;
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* citrus<skey_t, sval_t, RecMgr>::init() {

    node_t<skey_t, sval_t>* root = newNode(MAX_KEY, NULL);
    root->child[0] = newNode(MAX_KEY, NULL);
    return root;
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t citrus<skey_t, sval_t, RecMgr>::contains(node_t<skey_t, sval_t>* root, skey_t key) {
    urcu::readLock();
    node_t<skey_t, sval_t>* curr = root->child[0];
    sval_t result = NULL;
    skey_t ckey = curr->key;
    while (curr != NULL && ckey != key) {
        if (ckey > key)
            curr = curr->child[0];
        if (ckey < key){
            curr = curr->child[1];
            assert (curr != root->child[0]);
        }
        if (curr != NULL)
            ckey = curr->key;
    }
    if (curr != NULL) {
        result = curr->value;
    }
    urcu::readUnlock();
    return result;
}

template <typename skey_t, typename sval_t, class RecMgr>
bool citrus<skey_t, sval_t, RecMgr>::validate(node_t<skey_t, sval_t>* prev, int tag, node_t<skey_t, sval_t>* curr, int direction) {
    bool result;
    if (curr == NULL) {
        result = (!(prev->marked) && (prev->child[direction] == curr) && (prev->tag[direction] == tag));
    } else {
        result = (!(prev->marked) && !(curr->marked) && prev->child[direction] == curr);
    }
    return result;
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t citrus<skey_t, sval_t, RecMgr>::insert(node_t<skey_t, sval_t>* root, skey_t key, sval_t value) {
    while (true) {
        urcu::readLock();
        node_t<skey_t, sval_t>* prev = root;
        node_t<skey_t, sval_t>* curr = root->child[0];
        int direction = 0;
        skey_t ckey = curr->key;
        int tag;
        while (curr != NULL && ckey != key) {
            prev = curr;
            if (ckey > key) {
                curr = curr->child[0];
                direction = 0;
            }
            if (ckey < key) {
                curr = curr->child[1];
                direction = 1;
                assert (curr != root->child[0]);
            }
            if (curr != NULL)
                ckey = curr->key;
        }
        tag = prev->tag[direction];
        urcu::readUnlock();
        if (curr != NULL) {
            return curr->value;
        }
        mutex_lock(&(prev->lock));
        if (validate(prev, tag, curr, direction)) {
            node_t<skey_t, sval_t>* nnode = newNode(key, value);
            prev->child[direction] = nnode;
            mutex_unlock(&(prev->lock));
            return NULL;
        }
        mutex_unlock(&(prev->lock));
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
bool citrus<skey_t, sval_t, RecMgr>::remove(node_t<skey_t, sval_t>* root, skey_t key) {
    while (true) {
        urcu::readLock();
        node_t<skey_t, sval_t>* prev = root;
        node_t<skey_t, sval_t>* curr = root->child[0];
        int direction = 0;
        skey_t ckey = curr->key;
        while (curr != NULL && ckey != key) {
            prev = curr;
            if (ckey > key) {
                curr = curr->child[0];
                direction = 0;
            }
            if (ckey < key) {
                curr = curr->child[1];
                direction = 1;
            }
            if (curr != NULL)
                ckey = curr->key;
        }
        if (curr == NULL) {
            urcu::readUnlock(key);
            return false;
        }

        urcu::readUnlock(key);

        mutex_lock(&(prev->lock));
        mutex_lock(&(curr->lock));

        if (!validate(prev, 0, curr, direction)) {
            mutex_unlock(&(prev->lock));
            mutex_unlock(&(curr->lock));
            continue;
        }
        if (curr->child[0] == NULL) {
            curr->marked = true;
            prev->child[direction] = curr->child[1];
            if (prev->child[direction] == NULL) {
                prev->tag[direction]++;
            }
            mutex_unlock(&(prev->lock));
            mutex_unlock(&(curr->lock));
            return true;
        }
        if (curr->child[1] == NULL) {
            curr->marked = true;
            prev->child[direction] = curr->child[0];
            if (prev->child[direction] == NULL) {
                prev->tag[direction]++;
            }
            mutex_unlock(&(prev->lock));
            mutex_unlock(&(curr->lock));
            return true;
        }
        node_t<skey_t, sval_t>* prevSucc = curr;
        node_t<skey_t, sval_t>* succ = curr->child[1];

        node_t<skey_t, sval_t>* next = succ->child[0];
        while (next != NULL) {
            prevSucc = succ;
            succ = next;
            next = next->child[0];
        }
        int succDirection = 1;

        if (prevSucc != curr) {
            mutex_lock(&(prevSucc->lock));
            succDirection = 0;
        }
        mutex_lock(&(succ->lock));

        if (validate(prevSucc, 0, succ, succDirection) && validate(succ, succ->tag[0], NULL, 0)) {
            curr->marked = true;
            node_t<skey_t, sval_t>* nnode = newNode(succ->key, succ->value);
            nnode->child[0] = curr->child[0];
            nnode->child[1] = curr->child[1];
            mutex_lock(&(nnode->lock));
            prev->child[direction] = nnode;

            urcu::synchronize();

            succ->marked = true;

            if (prevSucc == curr) {
                nnode->child[1] = succ->child[1];
                if (nnode->child[1] == NULL) {
                    nnode->tag[1]++;
                }
            } else {
                prevSucc->child[0] = succ->child[1];
                if (prevSucc->child[0] == NULL) {
                    prevSucc->tag[0]++;
                }
            }
            mutex_unlock(&(prev->lock));
            mutex_unlock(&(nnode->lock));
            mutex_unlock(&(curr->lock));
            if (prevSucc != curr)
                mutex_unlock(&(prevSucc->lock));
            mutex_unlock(&(succ->lock));
            return true;
        }
        mutex_unlock(&(prev->lock));
        mutex_unlock(&(curr->lock));
        if (prevSucc != curr)
            mutex_unlock(&(prevSucc->lock));
        mutex_unlock(&(succ->lock));
    }
}

#endif /* CITRUS_IMP_H */
