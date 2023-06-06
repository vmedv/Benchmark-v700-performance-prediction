#pragma once

#include "saist_node.h"

template <typename K, typename V>
class GSATNodeHandler {
public:
    using NodePtrType = SAISTNode<K, V>*;

    class ChildIterator {
        NodePtrType node_;
        int at_;

    public:
        ChildIterator(NodePtrType node)
            : node_(node)
            , at_(0) {
        }

        bool HasNext() {
            while (at_ <= node_->keys_size && !node_->children[at_]) {
                ++at_;
            }
            return at_ != node_->keys_size + 1;
        }

        NodePtrType Next() {
            return node_->children[at_++];
        }
    };

    bool IsLeaf(NodePtrType node) {
        for (int index = 0; index <= node->keys_size; ++index) {
            if (node->children[index]) {
                return false;
            }
        }
        return true;
    }

    size_t GetNumKeys(NodePtrType node) {
        int size = 0;
        for (int index = 0; index < node->keys_size; ++index) {
            size += node->values[index].second > 0;
        }
        return size;
    }

    size_t GetSumKeys(NodePtrType node) {
        size_t sum = 0;
        for (int index = 0; index < node->keys_size; ++index) {
            sum += node->values[index].second > 0 ? node->keys[index] : 0;
        }
        return sum;
    }

    ChildIterator GetChildIterator(NodePtrType node) {
        return ChildIterator(node);
    }
};
