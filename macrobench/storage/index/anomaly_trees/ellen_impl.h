/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ellen_impl.h
 * Author: Maya Arbel-Raviv
 *
 * Created on June 1, 2017, 4:02 PM
 */

#ifndef ELLEN_IMPL_H
#define ELLEN_IMPL_H
#include "ellen.h"

static inline uint64_t GETFLAG(info_t<skey_t, sval_t>* ptr) {
    return ((uint64_t) ptr) & 3;
}

static inline uint64_t FLAG(info_t<skey_t, sval_t>* ptr, uint64_t flag) {
    return (((uint64_t) ptr) & 0xfffffffffffffffc) | flag;
}

static inline uint64_t UNFLAG(info_t<skey_t, sval_t>* ptr) {
    return (((uint64_t) ptr) & 0xfffffffffffffffc);
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* ellen<skey_t, sval_t, RecMgr>::create_node(skey_t key, sval_t value, bool_t is_leaf, int initializing) {
    volatile node_t<skey_t, sval_t> * new_node;
    new_node = (volatile node_t<skey_t, sval_t>*) tree_malloc::allocnode(sizeof (node_t<skey_t, sval_t>));
    if (new_node == NULL) {
        perror("malloc in bst create node");
        exit(1);
    }
    new_node->leaf = is_leaf;
    new_node->key = key;
    new_node->value = value;
    new_node->update = NULL;
    new_node->right = NULL;
    new_node->left = NULL;

    asm volatile("" :: : "memory");
    return (node_t<skey_t, sval_t>*) new_node;
}

template <typename skey_t, typename sval_t, class RecMgr>
node_t<skey_t, sval_t>* ellen<skey_t, sval_t, RecMgr>::bst_initialize() {
    node_t<skey_t, sval_t>* root;
    node_t<skey_t, sval_t>* i1;
    node_t<skey_t, sval_t>* i2;
    root = create_node(INF2, NULL, FALSE, 1);
    i1 = create_node(INF1, NULL, TRUE, 1);
    i2 = create_node(INF2, NULL, TRUE, 1);

    root->left = i1;
    root->right = i2;

    return root;
}

template <typename skey_t, typename sval_t, class RecMgr>
void ellen<skey_t, sval_t, RecMgr>::bst_init_local() {
}


template <typename skey_t, typename sval_t, class RecMgr>
void ellen<skey_t, sval_t, RecMgr>::bst_search(skey_t key, node_t<skey_t, sval_t>* root, search_result_t<skey_t, sval_t>* resptr) {
    resptr->l = root;
    resptr->pupdate = NULL;
    while (!(resptr->l->leaf)) {
        resptr->gp = resptr->p;
        resptr->p = resptr->l;
        resptr->gpupdate = resptr->pupdate;
        resptr->pupdate = resptr->p->update;
        if (key < resptr->l->key) {
            resptr->l = (node_t<skey_t, sval_t>*) resptr->p->left;
        } else {
            resptr->l = (node_t<skey_t, sval_t>*) resptr->p->right;
        }

    }
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t ellen<skey_t, sval_t, RecMgr>::bst_find(skey_t key, node_t<skey_t, sval_t>* root) {
    search_result_t<skey_t, sval_t> ___result;
    search_result_t<skey_t, sval_t> * result = &___result;
    bst_search(key, root, result);
    if (result->l->key == key) {
        return result->l->value;
    }
    return NULL;
}

template <typename skey_t, typename sval_t, class RecMgr>
info_t<skey_t, sval_t>* ellen<skey_t, sval_t, RecMgr>::create_iinfo_t(node_t<skey_t, sval_t>* p, node_t<skey_t, sval_t>* ni, node_t<skey_t, sval_t>* l) {
    volatile info_t<skey_t, sval_t> * new_info;
    new_info = (volatile info_t<skey_t, sval_t>*) tree_malloc::alloc(sizeof (info_t<skey_t, sval_t>));
    if (new_info == NULL) {
        perror("malloc in bst create node");
        exit(1);
    }
    new_info->iinfo.p = p;
    new_info->iinfo.new_internal = ni;
    new_info->iinfo.l = l;
    MEM_BARRIER;
    return (info_t<skey_t, sval_t>*) new_info;
}

template <typename skey_t, typename sval_t, class RecMgr>
info_t<skey_t, sval_t>* ellen<skey_t, sval_t, RecMgr>::create_dinfo_t(node_t<skey_t, sval_t>* gp, node_t<skey_t, sval_t>* p, node_t<skey_t, sval_t>* l, info_t<skey_t, sval_t>* u) {
    volatile info_t<skey_t, sval_t> * new_info;
    new_info = (volatile info_t<skey_t, sval_t>*) tree_malloc::alloc(sizeof (info_t<skey_t, sval_t>));
    if (new_info == NULL) {
        perror("malloc in bst create node");
        exit(1);
    }

    new_info->dinfo.gp = gp;
    new_info->dinfo.p = p;
    new_info->dinfo.l = l;
    new_info->dinfo.pupdate = u;
    MEM_BARRIER;
    return (info_t<skey_t, sval_t>*) new_info;
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t ellen<skey_t, sval_t, RecMgr>::bst_insert(skey_t key, sval_t value, node_t<skey_t, sval_t>* root) {
    node_t<skey_t, sval_t> * new_internal;
    node_t<skey_t, sval_t> *new_sibling;

    node_t<skey_t, sval_t> * new_node = NULL;

    info_t<skey_t, sval_t>* result;

    info_t<skey_t, sval_t>* op;
    search_result_t<skey_t, sval_t> ___search_result = {NULL,NULL,NULL,NULL,NULL};
    search_result_t<skey_t, sval_t>* search_result = &___search_result;

    while (1) {
        bst_search(key, root, search_result);
        if (search_result->l->key == key) {
#ifdef URCU_GC
            if (new_node != NULL) {
                free_node(new_node);
            }
#endif
            return search_result->l->value;
        }
        if (GETFLAG(search_result->pupdate) != STATE_CLEAN) {
            bst_help(search_result->pupdate);
        } else {
            if (new_node == NULL) {
                new_node = create_node(key, value, TRUE, 0);
            }
            new_sibling = create_node(search_result->l->key, search_result->l->value, TRUE, 0);
            new_internal = create_node(max(key, search_result->l->key), 0, FALSE, 0);

            if (new_node->key < new_sibling->key) {
                new_internal->left = new_node;
                new_internal->right = new_sibling;
            } else {
                new_internal->left = new_sibling;
                new_internal->right = new_node;
            }
            op = create_iinfo_t(search_result->p, new_internal, search_result->l);
            result = CAS_PTR(&(search_result->p->update), search_result->pupdate, FLAG(op, STATE_IFLAG));
            if (result == search_result->pupdate) {
                bst_help_insert(op);
#ifdef URCU_GC
                if (UNFLAG(result) != 0) {
                    free_node((void*) UNFLAG(search_result->pupdate));
                }
#endif
                return NULL;
            } else {
                bst_help(result);
#ifdef URCU_GC
                //I think these can be freed immediatly or reused??? 
                //Ask Trevor - he agrees
                free(new_sibling);
                free(new_internal);
                free(op);
#endif 
            }
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
void ellen<skey_t, sval_t, RecMgr>::bst_help_insert(info_t<skey_t, sval_t> * op) {
#ifndef URCU_GC
    bst_cas_child(op->iinfo.p, op->iinfo.l, op->iinfo.new_internal);
    //iinfo_t<skey_t, sval_t>* cl = (iinfo_t*) UNFLAG(op);
    void* UNUSED dummy = CAS_PTR(&(op->iinfo.p->update), FLAG(op, STATE_IFLAG), FLAG(op, STATE_CLEAN));
#else
    int i = bst_cas_child(op->iinfo.p, op->iinfo.l, op->iinfo.new_internal);
    iinfo_t<skey_t, sval_t>* cl = (iinfo_t*) UNFLAG(op);
    void* UNUSED dummy = CAS_PTR(&(op->iinfo.p->update), FLAG(op, STATE_IFLAG), FLAG(op, STATE_CLEAN));
    if (i) {
        info_t<skey_t, sval_t>* uop = (info_t<skey_t, sval_t>*) UNFLAG(op);
        free_node(uop->iinfo.l);
    }
#endif
}

template <typename skey_t, typename sval_t, class RecMgr>
sval_t ellen<skey_t, sval_t, RecMgr>::bst_delete(skey_t key, node_t<skey_t, sval_t>* root) {
    info_t<skey_t, sval_t>* result;
    info_t<skey_t, sval_t>* op;
    sval_t found_value;
    
    search_result_t<skey_t, sval_t> ___search_result;
    search_result_t<skey_t, sval_t>* search_result = &___search_result;

    while (1) {
        bst_search(key, root, search_result);
        if (search_result->l->key != key) {
            return 0;
        }
        found_value = search_result->l->value;
        if (GETFLAG(search_result->gpupdate) != STATE_CLEAN) {
            bst_help(search_result->gpupdate);
        } else if (GETFLAG(search_result->pupdate) != STATE_CLEAN) {
            bst_help(search_result->pupdate);
        } else {
            op = create_dinfo_t(search_result->gp, search_result->p, search_result->l, search_result->pupdate);
            result = CAS_PTR(&(search_result->gp->update), search_result->gpupdate, FLAG(op, STATE_DFLAG));
            if (result == search_result->gpupdate) {
                if (bst_help_delete(op) == TRUE) {
#ifdef URCU_GC
                    free_node((void*) UNFLAG(search_result->gpupdate));
#endif
                    return found_value;
                }
            } else {
                bst_help(result);
#ifdef URCU_GC
                //I think these can be freed immediatly or reused??? 
                //Ask Trevor
                free(op);
#endif 
            }
        }
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
bool_t ellen<skey_t, sval_t, RecMgr>::bst_help_delete(info_t<skey_t, sval_t>* op) {
    info_t<skey_t, sval_t>* result;
    result = CAS_PTR(&(op->dinfo.p->update), op->dinfo.pupdate, FLAG(op, STATE_MARK));
    if ((result == op->dinfo.pupdate) || (result == ((info_t<skey_t, sval_t>*)FLAG(op, STATE_MARK)))) {
#ifdef URCU_GC
        if ((result == op->dinfo.pupdate) && ((void*) UNFLAG(result) != NULL)) {
            free_node((void*) UNFLAG(result));
        }
#endif
        bst_help_marked(op);
        return TRUE;
    } else {
        bst_help(result);
        void* UNUSED dummy = CAS_PTR(&(op->dinfo.gp->update), FLAG(op, STATE_DFLAG), FLAG(op, STATE_CLEAN));
        return FALSE;
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
void ellen<skey_t, sval_t, RecMgr>::bst_help_marked(info_t<skey_t, sval_t>* op) {
    node_t<skey_t, sval_t>* other;
    if (op->dinfo.p->right == op->dinfo.l) {
        other = (node_t<skey_t, sval_t>*) op->dinfo.p->left;
    } else {
        other = (node_t<skey_t, sval_t>*) op->dinfo.p->right;
    }

#ifndef URCU_GC
    bst_cas_child(op->dinfo.gp, op->dinfo.p, other);
    void* UNUSED dummy = CAS_PTR(&(op->dinfo.gp->update), FLAG(op, STATE_DFLAG), FLAG(op, STATE_CLEAN));
#else
    int i = bst_cas_child(op->dinfo.gp, op->dinfo.p, other);
    void* UNUSED dummy = CAS_PTR(&(op->dinfo.gp->update), FLAG(op, STATE_DFLAG), FLAG(op, STATE_CLEAN));
    if (i) {
        info_t<skey_t, sval_t>* opu = (info_t<skey_t, sval_t>*) UNFLAG(op);
        free_node(opu->dinfo.l);
        free_node(opu->dinfo.p);
    }
#endif
}

template <typename skey_t, typename sval_t, class RecMgr>
void ellen<skey_t, sval_t, RecMgr>::bst_help(info_t<skey_t, sval_t>* u) {
    if (GETFLAG(u) == STATE_IFLAG) {
        bst_help_insert((info_t<skey_t, sval_t>*) UNFLAG(u));
    } else if (GETFLAG(u) == STATE_MARK) {
        bst_help_marked((info_t<skey_t, sval_t>*) UNFLAG(u));
    } else if (GETFLAG(u) == STATE_DFLAG) {
        bst_help_delete((info_t<skey_t, sval_t>*) UNFLAG(u));
    }
}

template <typename skey_t, typename sval_t, class RecMgr>
int ellen<skey_t, sval_t, RecMgr>::bst_cas_child(node_t<skey_t, sval_t>* parent, node_t<skey_t, sval_t>* old, node_t<skey_t, sval_t>* nnode) {
    if (nnode->key < parent->key) {
        if (CAS_PTR(&(parent->left), old, nnode) == old) return 1;
        return 0;
    } else {
        if (CAS_PTR(&(parent->right), old, nnode) == old) return 1;
        return 0;
    }
}



#endif /* ELLEN_IMPL_H */

