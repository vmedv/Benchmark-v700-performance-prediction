#pragma once

#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <utility>
#include <vector>
#include <tuple>

#include "../../common/error.h"

#include "saist_node.h"
#include "saist_node_handler.h"

#ifdef KEY_DEPTH_TOTAL_STAT
extern int64_t key_depth_total_sum__;
extern int64_t key_depth_total_cnt__;
#endif

#ifdef KEY_DEPTH_STAT
extern int64_t* key_depth_sum__;
extern int64_t* key_depth_cnt__;
#endif

template <typename K, typename V>
class SAIST {
public:
    static constexpr int kLeafSize = 16;
    static constexpr int64_t kMinRebuildBound = 100;
    static constexpr double kRebuildFactor = 1.25;

    using NODE = SAISTNode<K, V>;
    using NodeHandler = SAISTNodeHandler<K, V>;
    static_assert(std::is_same<NODE*, typename NodeHandler::NodePtrType>::value);

private:
    const V no_value_;
    const double alpha_;
    const K left_bound_;
    const K right_bound_;
    const int leaf_size_;
    const int64_t min_rebuild_bound_;
    const double rebuild_factor_;
    NODE* root_;

public:
    SAIST(const V& no_value, double alpha, const K& left_bound, const K& right_bound, int leaf_size,
          int64_t min_rebuild_bound, double rebuild_factor)
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
        Check(rebuild_factor_ > 1);
    }

    SAIST(const V& no_value, double alpha, const K& left_bound, const K& right_bound)
        : SAIST(no_value, alpha, left_bound, right_bound, kLeafSize, kMinRebuildBound,
                kRebuildFactor) {
    }

    ~SAIST() {
        delete root_;
    }

    V Find(const K& key) {
#if defined KEY_DEPTH_TOTAL_STAT || defined KEY_DEPTH_STAT
        int d__ = 0;
#endif
        assert(root_);

        NODE* node = root_;
        NODE** node_at = &root_;

        NODE* rebuild_node = nullptr;
        NODE** rebuild_node_at = nullptr;

        V result = no_value_;

        while (node) {
            if (node->Access() && !rebuild_node) {
                rebuild_node = node;
                rebuild_node_at = node_at;
            }

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
                if (node->values[index].second < 0) {
                    --node->values[index].second;
                } else {
                    ++node->values[index].second;
                    result = node->values[index].first;
                }
                break;
            }
#if defined KEY_DEPTH_TOTAL_STAT || defined KEY_DEPTH_STAT
            ++d__;
#endif
            node_at = &node->children[index];
            node = node->children[index];
        }

        if (rebuild_node) {
            *rebuild_node_at = RebuildTree(rebuild_node);
        }

        return result;
    }

    bool Contains(const K& key) {
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

            if (node->Access() && !rebuild_node) {
                rebuild_node = node;
                rebuild_node_at = node_at;
            }

            int index = node->FindKey(key);
            if (node->IsKeyFound(index, key)) {
                if (node->values[index].second < 0) {
                    node->values[index].second = -node->values[index].second;
                    node->values[index].first = value;
                } else {
                    result = node->values[index].first;
                }
                ++node->values[index].second;
                break;
            }

            if (!node->children[index]) {
                if (node->keys_size < node->capacity) {
                    node->Insert(index, key, value, 1);
                } else {
                    const K& left_bound = index == 0 ? node->left_bound : node->keys[index - 1];
                    const K& right_bound =
                        index == node->keys_size ? node->right_bound : node->keys[index];
                    NODE* child = new NODE(left_bound, right_bound, leaf_size_, min_rebuild_bound_);
                    child->Insert(0, key, value, 1);
                    ++child->total_asize;
                    ++child->total_accesses;

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
            if (node->Access() && !rebuild_node) {
                rebuild_node = node;
                rebuild_node_at = node_at;
            }

            int index = node->FindKey(key);
            if (node->IsKeyFound(index, key)) {
                if (node->values[index].second > 0) {
                    result = node->values[index].first;
                    node->values[index].second = -node->values[index].second;
                }
                --node->values[index].second;
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
    int64_t GetBound(int64_t total_accesses) {
        return ceil(pow(total_accesses, 1 - alpha_));
    }

    int GetIdsSize(int64_t total_accesses) {
        return ceil(pow(total_accesses, alpha_));
    }

    NODE* RebuildTree(NODE* node) {
        std::vector<std::tuple<K, V, int>> data;
        data.reserve(node->total_asize);

        CollectNonErased(node, data);

        std::vector<int64_t> psum_accesses(data.size() + 1);
        psum_accesses[0] = 0;
        for (size_t index = 0; index < data.size(); ++index) {
            psum_accesses[index + 1] = psum_accesses[index] + std::get<2>(data[index]);
        }

        NODE* result = BuildIdealTree(node->left_bound, node->right_bound, 0, data.size(), data,
                                      psum_accesses);

        delete node;

        return result;
    }

    static void CollectNonErased(NODE* node, std::vector<std::tuple<K, V, int>>& data) {
        assert(node);

        if (node->children[0]) {
            CollectNonErased(node->children[0], data);
        }

        for (int index = 0; index < node->keys_size; ++index) {
            if (node->values[index].second > 0) {
                data.emplace_back(std::move(node->keys[index]),
                                  std::move(node->values[index].first), node->values[index].second);
            }
            if (node->children[index + 1]) {
                CollectNonErased(node->children[index + 1], data);
            }
        }
    }

    // [left, right)
    NODE* BuildIdealTree(const K& left_bound, const K& right_bound, int left, int right,
                         std::vector<std::tuple<K, V, int>>& data,
                         std::vector<int64_t>& psum_accesses) {
        const int size = right - left;
        if (size <= 0) {
            return nullptr;
        }

        const int64_t total_accesses = psum_accesses[right] - psum_accesses[left];
        const int64_t rebuild_bound =
            std::max(min_rebuild_bound_, static_cast<int64_t>(rebuild_factor_ * total_accesses));

        if (size <= leaf_size_) {
            NODE* node = new NODE(left_bound, right_bound, leaf_size_, rebuild_bound);
            node->total_asize = node->keys_size = size;
            node->total_accesses = total_accesses;
            for (int index = 0; index < size; ++index) {
                node->keys[index] = std::move(std::get<0>(data[left + index]));
                node->values[index].first = std::move(std::get<1>(data[left + index]));
                node->values[index].second = std::get<2>(data[left + index]);
            }
            return node;
        }

        const int64_t bound = GetBound(total_accesses);

        std::vector<int> key_indices;
        key_indices.reserve(leaf_size_);

        const int cleft = left;
        const int cright = right;

        while (true) {
            int l = left;
            int r = right;
            while (r - l > 1) {
                int m = (l + r) >> 1;
                if (psum_accesses[m] - psum_accesses[left] < bound) {
                    l = m;
                } else {
                    r = m;
                }
            }

            if (r == right) {
                if (key_indices.empty()) {
                    key_indices.push_back(r - 1);
                }
                break;
            }

            key_indices.push_back(r - 1);
            left = r;
        }

        const int keys_size = key_indices.size();

        NODE* node = new NODE(left_bound, right_bound, keys_size, rebuild_bound);
        node->total_asize = size;
        node->keys_size = keys_size;
        node->total_accesses = total_accesses;

        left = cleft;
        for (int index = 0; index < keys_size; ++index) {
            right = key_indices[index];

            node->keys[index] = std::move(std::get<0>(data[right]));
            node->values[index].first = std::move(std::get<1>(data[right]));
            node->values[index].second = std::get<2>(data[right]);

            const K& child_left_bound = index == 0 ? left_bound : node->keys[index - 1];
            const K& child_right_bound = node->keys[index];

            node->children[index] = BuildIdealTree(child_left_bound, child_right_bound, left, right,
                                                   data, psum_accesses);
            left = right + 1;
        }
        node->children[keys_size] = BuildIdealTree(node->keys[keys_size - 1], right_bound, left,
                                                   cright, data, psum_accesses);

        node->InitializeIds(GetIdsSize(total_accesses));

        return node;
    }

    std::pair<int, int64_t> Validate(const NODE* node, const K& left_bound,
                                     const K& right_bound) const {
        if (!node) {
            return {0, 0};
        }

        Check(node->left_bound == left_bound);
        Check(node->right_bound == right_bound);

        int total_size = 0;
        int64_t total_accesses = 0;

        for (int index = 0; index <= node->keys_size; ++index) {
            const K& child_left_bound = index == 0 ? left_bound : node->keys[index - 1];
            const K& child_right_bound = index == node->keys_size ? right_bound : node->keys[index];

            Check(child_left_bound < child_right_bound);

            if (index < node->keys_size) {
                Check(node->values[index].first != no_value_);
                total_size += node->values[index].second > 0;
                total_accesses += std::abs(node->values[index].second);
            }

            if (node->children) {
                auto [child_total_size, child_total_accesses] =
                    Validate(node->children[index], child_left_bound, child_right_bound);
                total_size += child_total_size;
                total_accesses += child_total_accesses;
            }
        }

        Check(total_size <= node->total_asize);
        Check(total_accesses <= node->total_accesses);

        return {total_size, total_accesses};
    }
};
