#pragma once

#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <utility>
#include <vector>

#include "../../common/error.h"

#include "ist_node.h"
#include "ist_node_handler.h"

#ifdef KEY_DEPTH_TOTAL_STAT
extern int64_t key_depth_total_sum__;
extern int64_t key_depth_total_cnt__;
#endif

#ifdef KEY_DEPTH_STAT
extern int64_t* key_depth_sum__;
extern int64_t* key_depth_cnt__;
#endif

template <typename K, typename V>
class IST {
public:
    static constexpr int kLeafSize = 32;
    static constexpr int kMinRebuildBound = 225;
    static constexpr double kRebuildFactor = 0.75;

    using NODE = ISTNode<K, V>;
    using NodeHandler = ISTNodeHandler<K, V>;
    static_assert(std::is_same<NODE*, typename NodeHandler::NodePtrType>::value);

private:
    const V no_value_;
    const double alpha_;
    const K left_bound_;
    const K right_bound_;
    const int leaf_size_;
    const int min_rebuild_bound_;
    const double rebuild_factor_;
    NODE* root_;

public:
    IST(const V& no_value, double alpha, const K& left_bound, const K& right_bound, int leaf_size,
        int min_rebuild_bound, double rebuild_factor)
        : no_value_(no_value)
        , alpha_(alpha)
        , left_bound_(left_bound)
        , right_bound_(right_bound)
        , leaf_size_(leaf_size)
        , min_rebuild_bound_(min_rebuild_bound)
        , rebuild_factor_(rebuild_factor)
        , root_(new NODE(left_bound_, right_bound_, leaf_size_, min_rebuild_bound_)) {
        Check(0 <= alpha_ && alpha_ <= 0.5);
        Check(left_bound < right_bound);
        Check(leaf_size_ > 0);
        Check(min_rebuild_bound_ > 1);
        Check(rebuild_factor > 0);
    }

    IST(const V& no_value, double alpha, const K& left_bound, const K& right_bound)
        : IST(no_value, alpha, left_bound, right_bound, kLeafSize, kMinRebuildBound,
              kRebuildFactor) {
    }

    ~IST() {
        delete root_;
    }

    V Find(const K& key) const {
#if defined KEY_DEPTH_TOTAL_STAT || defined KEY_DEPTH_STAT
        int d__ = 0;
#endif
        assert(root_);

        NODE* node = root_;
        while (node) {
            int index = node->FindKey(key);
            if (node->IsKeyFound(index, key)) {
#ifdef KEY_DEPTH_TOTAL_STAT
                key_depth_total_sum__ += d__;
                ++key_depth_total_cnt__;
#endif
#ifdef KEY_DEPTH_STAT
                key_depth_sum__[key] += d__;
                ++key_depth_cnt__[key];
#endif
                return node->erased[index] ? no_value_ : node->values[index];
            }
#if defined KEY_DEPTH_TOTAL_STAT || defined KEY_DEPTH_STAT
            ++d__;
#endif
            node = node->children[index];
        }
        return no_value_;
    }

    bool Contains(const K& key) const {
        return Find(key) != no_value_;
    }

    V InsertIfAbsent(const K& key, const V& value) {
        assert(root_);
        assert(root_->left_bound <= key && key < root_->right_bound);

        NODE* node = root_;
        NODE** node_at = &root_;

        NODE* rebuild_node = nullptr;
        NODE** rebuild_node_at = nullptr;

        V result = no_value_;

        while (true) {
            ++node->total_asize;

            if (node->Count() && !rebuild_node) {
                rebuild_node = node;
                rebuild_node_at = node_at;
            }

            int index = node->FindKey(key);
            if (node->IsKeyFound(index, key)) {
                if (node->erased[index]) {
                    node->values[index] = value;
                    node->erased[index] = false;
                } else {
                    result = node->values[index];
                }
                break;
            }

            if (!node->children[index]) {
                if (node->keys_size < node->capacity) {
                    node->Insert(index, key, value);
                } else {
                    const K& left_bound = index == 0 ? node->left_bound : node->keys[index - 1];
                    const K& right_bound =
                        index == node->keys_size ? node->right_bound : node->keys[index];
                    NODE* child = new NODE(left_bound, right_bound, leaf_size_, min_rebuild_bound_);
                    child->Insert(0, key, value);
                    ++child->total_asize;
                    ++child->counter;

                    node->children[index] = child;
                }
                break;
            }
            node_at = &node->children[index];
            node = node->children[index];
        }

        if (rebuild_node) {
            *rebuild_node_at = RebuildTree(rebuild_node);
        }

        return result;
    }

    V Erase(const K& key) {
        assert(root_);

        NODE* node = root_;
        NODE** node_at = &root_;

        NODE* rebuild_node = nullptr;
        NODE** rebuild_node_at = nullptr;

        V result = no_value_;

        while (node) {
            if (node->Count() && !rebuild_node) {
                rebuild_node = node;
                rebuild_node_at = node_at;
            }

            int index = node->FindKey(key);
            if (node->IsKeyFound(index, key)) {
                if (!node->erased[index]) {
                    result = node->values[index];
                    node->erased[index] = true;
                }
                break;
            }
            node_at = &node->children[index];
            node = node->children[index];
        }

        if (rebuild_node) {
            *rebuild_node_at = RebuildTree(rebuild_node);
        }

        return result;
    }

    void Validate() const {
        Validate(root_, left_bound_, right_bound_);
    }

    NODE* GetRoot() {
        return root_;
    }

    NodeHandler* GetNodeHandler() {
        return new NodeHandler();
    }

private:
    int GetIdsSize(int keys_size) {
        return ceil(pow(keys_size, 1 - alpha_));
    }

    NODE* RebuildTree(NODE* node) {
        std::vector<std::pair<K, V>> data;
        data.reserve(node->total_asize);

        CollectNonErased(node, data);

        NODE* result = BuildIdealTree(node->left_bound, node->right_bound, 0, data.size(), data);

        delete node;

        return result;
    }

    static void CollectNonErased(NODE* node, std::vector<std::pair<K, V>>& data) {
        assert(node);

        if (node->children[0]) {
            CollectNonErased(node->children[0], data);
        }

        for (int index = 0; index < node->keys_size; ++index) {
            if (!node->erased[index]) {
                data.emplace_back(std::move(node->keys[index]), std::move(node->values[index]));
            }
            if (node->children[index + 1]) {
                CollectNonErased(node->children[index + 1], data);
            }
        }
    }

    // [left, right)
    NODE* BuildIdealTree(const K& left_bound, const K& right_bound, int left, int right,
                         std::vector<std::pair<K, V>>& data) {
        const int size = right - left;
        if (size <= 0) {
            return nullptr;
        }

        if (size <= leaf_size_) {
            NODE* node = new NODE(left_bound, right_bound, leaf_size_, min_rebuild_bound_);
            node->total_asize = node->keys_size = size;
            for (int index = 0; index < size; ++index) {
                node->keys[index] = std::move(data[left + index].first);
                node->values[index] = std::move(data[left + index].second);
                node->erased[index] = false;
            }
            return node;
        }

        const int rebuild_bound =
            std::max(min_rebuild_bound_, static_cast<int>(rebuild_factor_ * size));
        const int step = sqrt(size);

        NODE* node = new NODE(left_bound, right_bound, step, rebuild_bound);
        node->total_asize = size;
        node->keys_size = step;

        for (int index = 0; index < step; ++index) {
            int child_left = left + index * step;
            int child_right = left + (index + 1) * step - 1;

            node->keys[index] = std::move(data[child_right].first);
            node->values[index] = std::move(data[child_right].second);
            node->erased[index] = false;

            const K& child_left_bound = index == 0 ? left_bound : node->keys[index - 1];
            const K& child_right_bound = node->keys[index];

            node->children[index] =
                BuildIdealTree(child_left_bound, child_right_bound, child_left, child_right, data);
        }
        node->children[step] =
            BuildIdealTree(node->keys[step - 1], right_bound, left + step * step, right, data);

        node->InitializeIds(GetIdsSize(step));

        return node;
    }

    int Validate(const NODE* node, const K& left_bound, const K& right_bound) const {
        if (!node) {
            return 0;
        }

        Check(node->left_bound == left_bound);
        Check(node->right_bound == right_bound);

        int total_size = 0;
        for (int index = 0; index <= node->keys_size; ++index) {
            const K& child_left_bound = index == 0 ? left_bound : node->keys[index - 1];
            const K& child_right_bound = index == node->keys_size ? right_bound : node->keys[index];

            Check(child_left_bound < child_right_bound);
            CheckIfTrue(index < node->keys_size, node->values[index] != no_value_);

            total_size += index < node->keys_size && !node->erased[index];

            total_size += Validate(node->children[index], child_left_bound, child_right_bound);
        }

        Check(total_size <= node->total_asize);

        return total_size;
    }
};
