#pragma once

#include "sat_node.h"

template <typename K, typename V>
class SATNodeHandler {
public:
    using NodePtrType = SATNode<K, V>*;

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
        return node->accesses > 0;
    }

    size_t GetSumKeys(NodePtrType node) {
        return node->accesses > 0 ? node->key : 0;
    }

    ChildIterator GetChildIterator(NodePtrType node) {
        return ChildIterator(node);
    }
};
