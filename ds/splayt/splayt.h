#pragma once

#include <assert.h>
#include <utility>

#include "../../common/error.h"

#include "splay_node.h"
#include "splay_node_handler.h"

#ifdef KEY_DEPTH_TOTAL_STAT
extern int64_t key_depth_total_sum__;
extern int64_t key_depth_total_cnt__;
#endif

#ifdef KEY_DEPTH_STAT
extern int64_t* key_depth_sum__;
extern int64_t* key_depth_cnt__;
#endif

template <typename K, typename V>
class SplayT {
public:
    using NODE = SplayNode<K, V>;
    using NodeHandler = SplayNodeHandler<K, V>;
    static_assert(std::is_same<NODE*, typename NodeHandler::NodePtrType>::value);

private:
    const V no_value_;
    NODE* root_;

public:
    explicit SplayT(const V& no_value)
        : no_value_(no_value)
        , root_(nullptr) {
    }

    ~SplayT() {
        Free(root_);
    }

    V Find(const K& key) {
        auto [node, parent] = FindNodeAndParent(key);
        if (node) {
            Splay(node);
            return node->value;
        } else {
            Splay(parent);
            return no_value_;
        }
    }

    bool Contains(const K& key) {
        return Find(key) != no_value_;
    }

    V InsertIfAbsent(const K& key, const V& value) {
        auto [node, parent] = FindNodeAndParent(key);

        V result = no_value_;

        if (node) {
            result = node->value;
        } else {
            node = new NODE(key, value);
            AddNode(parent, node);
        }

        Splay(node);

        return result;
    }

    V Erase(const K& key) {
        auto [node, parent] = FindNodeAndParent(key);

        if (!node) {
            Splay(parent);
            return no_value_;
        }

        DoSplay(node);

        V result = std::move(node->value);
        auto left = node->left;
        auto right = node->right;

        delete node;

        NODE* new_root = nullptr;

        if (left) {
            left->parent = nullptr;
            new_root = Max(left);
            DoSplay(new_root);
            SetRight(new_root, right);
        } else if (right) {
            right->parent = nullptr;
            new_root = Min(right);
            DoSplay(new_root);
            SetLeft(new_root, left);
        }

        root_ = new_root;

        return result;
    }

    void Validate() const {
        Validate(root_, nullptr, nullptr);
    }

    NODE* GetRoot() {
        return root_;
    }

    NodeHandler* GetNodeHandler() {
        return new NodeHandler();
    }

private:
    std::pair<NODE*, NODE*> FindNodeAndParent(const K& key) const {
#if defined KEY_DEPTH_TOTAL_STAT || defined KEY_DEPTH_STAT
        int d__ = 0;
#endif
        NODE* parent = nullptr;
        NODE* node = root_;
        while (node && node->key != key) {
#if defined KEY_DEPTH_TOTAL_STAT || defined KEY_DEPTH_STAT
            ++d__;
#endif
            parent = node;
            node = key < node->key ? node->left : node->right;
        }
#ifdef KEY_DEPTH_TOTAL_STAT
        if (node && node->key == key) {
            key_depth_total_sum__ += d__;
            ++key_depth_total_cnt__;
        }
#endif
#ifdef KEY_DEPTH_STAT
        if (node && node->key == key) {
            key_depth_sum__[key] += d__;
            ++key_depth_cnt__[key];
        }
#endif
        return {node, parent};
    }

    void AddNode(NODE* parent, NODE* node) {
        if (parent) {
            if (node->key < parent->key) {
                SetLeft(parent, node);
            } else {
                SetRight(parent, node);
            }
        } else {
            root_ = node;
        }
    }

    void Splay(NODE* node) {
        DoSplay(node);
        root_ = node;
    }

    void Validate(NODE* node, const K* left, const K* right) const {
        if (!node) {
            return;
        }

        CheckIfTrue(left, *left < node->key);
        CheckIfTrue(right, node->key < *right);
        Check(node->value != no_value_);

        Validate(node->left, left, &node->key);
        Validate(node->right, &node->key, right);
    }

    static NODE* Min(NODE* node) {
        assert(node);
        while (node->left) {
            node = node->left;
        }
        return node;
    }

    static NODE* Max(NODE* node) {
        assert(node);
        while (node->right) {
            node = node->right;
        }
        return node;
    }

    static void Free(NODE* node) {
        if (!node) {
            return;
        }
        Free(node->left);
        Free(node->right);
        delete node;
    }

    static void DoSplay(NODE* node) {
        while (GetParent(node)) {
            auto parent = node->parent;
            auto grand_parent = GetParent(parent);
            if (grand_parent) {
                if (parent->right == node) {
                    if (grand_parent->right == parent) {
                        RotateLeft(parent);
                        RotateLeft(node);
                    } else {
                        RotateLeft(node);
                        RotateRight(node);
                    }
                } else {
                    if (grand_parent->left == parent) {
                        RotateRight(parent);
                        RotateRight(node);
                    } else {
                        RotateRight(node);
                        RotateLeft(node);
                    }
                }
            } else {
                if (parent->right == node) {
                    RotateLeft(node);
                } else {
                    RotateRight(node);
                }
            }
        }
    }

    static void RotateRight(NODE* node) {
        assert(node);
        NODE* parent = node->parent;
        UpdateParents(parent, node);
        SetLeft(parent, node->right);
        SetRight(node, parent);
    }

    static void RotateLeft(NODE* node) {
        assert(node);
        NODE* parent = node->parent;
        UpdateParents(parent, node);
        SetRight(parent, node->left);
        SetLeft(node, parent);
    }

    static void UpdateParents(NODE* parent, NODE* node) {
        NODE* grand_parent = GetParent(parent);
        if (GetRight(grand_parent) == parent) {
            SetRight(grand_parent, node);
        } else {
            SetLeft(grand_parent, node);
        }
        SetParent(parent, node);
    }

    static inline NODE* GetLeft(NODE* node) {
        return node ? node->left : nullptr;
    }

    static inline NODE* GetRight(NODE* node) {
        return node ? node->right : nullptr;
    }

    static inline NODE* GetParent(NODE* node) {
        return node ? node->parent : nullptr;
    }

    static inline void SetLeft(NODE* node, NODE* left) {
        if (node) {
            node->left = left;
        }
        SetParent(left, node);
    }

    static inline void SetRight(NODE* node, NODE* right) {
        if (node) {
            node->right = right;
        }
        SetParent(right, node);
    }

    static inline void SetParent(NODE* node, NODE* parent) {
        if (node) {
            node->parent = parent;
        }
    }
};
