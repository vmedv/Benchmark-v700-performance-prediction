#pragma once

#include "bt_node.h"

template <typename K, typename V, size_t kMaxKeys>
class BTNodeHandler {
public:
    using NodePtrType = BTNode<K, V, kMaxKeys>*;

    class ChildIterator {
    public:
        ChildIterator(NodePtrType node)
            : node_(node)
            , at_(0) {
        }

        bool HasNext() {
            while (at_ <= node_->size && !node_->children[at_]) {
                ++at_;
            }
            return at_ != node_->size + 1;
        }

        NodePtrType Next() {
            return node_->children[at_++];
        }

    private:
        NodePtrType node_;
        int at_;
    };

    bool IsLeaf(NodePtrType node) {
        for (size_t index = 0; index <= node->size; ++index) {
            if (node->children[index]) {
                return false;
            }
        }
        return true;
    }

    size_t GetNumKeys(NodePtrType node) {
        return node->size;
    }

    size_t GetSumKeys(NodePtrType node) {
        size_t sum = 0;
        for (int index = 0; index < node->size; ++index) {
            sum += node->keys[index];
        }
        return sum;
    }

    ChildIterator GetChildIterator(NodePtrType node) {
        return ChildIterator(node);
    }
};
