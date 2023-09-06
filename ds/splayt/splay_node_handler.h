#pragma once

#include <cstddef>

#include "splay_node.h"

template <typename K, typename V>
class SplayNodeHandler {
public:
    using NodePtrType = SplayNode<K, V>*;

    class ChildIterator {
        NodePtrType node_;
        int at_;

    public:
        ChildIterator(NodePtrType node)
            : node_(node)
            , at_(0) {
        }

        bool HasNext() {
            if (at_ == 0 && !node_->left) {
                ++at_;
            }
            if (at_ == 1 && !node_->right) {
                ++at_;
            }
            return at_ != 2;
        }

        NodePtrType Next() {
            return at_++ ? node_->right : node_->left;
        }
    };

    bool IsLeaf(NodePtrType node) {
        return !(node->left || node->right);
    }

    size_t GetNumKeys(NodePtrType node) {
        return 1;
    }

    size_t GetSumKeys(NodePtrType node) {
        return node->key;
    }

    ChildIterator GetChildIterator(NodePtrType node) {
        return ChildIterator(node);
    }
};
