#pragma once

#include "ist_node.h"

template <typename K, typename V>
class ISTNodeHandler {
public:
    using NodePtrType = ISTNode<K, V>*;

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
            size += !node->erased[index];
        }
        return size;
    }

    size_t GetSumKeys(NodePtrType node) {
        size_t sum = 0;
        for (int index = 0; index < node->keys_size; ++index) {
            sum += !node->erased[index] ? node->keys[index] : 0;
        }
        return sum;
    }

    ChildIterator GetChildIterator(NodePtrType node) {
        return ChildIterator(node);
    }
};
