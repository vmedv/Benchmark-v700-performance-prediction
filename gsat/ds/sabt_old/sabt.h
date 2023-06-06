#pragma once

#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <utility>
#include <vector>

#include "../../common/error.h"

#include "sabt_node.h"
#include "sabt_node_handler.h"

#ifdef KEY_DEPTH_TOTAL_STAT
extern int64_t key_depth_total_sum__;
extern int64_t key_depth_total_cnt__;
#endif

#ifdef KEY_DEPTH_STAT
extern int64_t* key_depth_sum__;
extern int64_t* key_depth_cnt__;
#endif

template <typename K, typename V, size_t kMinKeys>
class SABT {
public:
    static constexpr int64_t kMinRebuildBound = 100;
    static constexpr double kRebuildFactor = 1.25;

    static_assert(kMinKeys > 0, "kMinKeys must be > 0");
    static constexpr int kMaxKeys = 2 * kMinKeys;

    using NODE = SABTNode<K, V, kMaxKeys>;
    using NodeHandler = SABTNodeHandler<K, V, kMaxKeys>;
    static_assert(std::is_same<NODE*, typename NodeHandler::NodePtrType>::value);

private:
    const V no_value_;
    const int64_t min_rebuild_bound_;
    const double rebuild_factor_;
    NODE* root_;

public:
    SABT(const V& no_value, int64_t min_rebuild_bound, double rebuild_factor)
        : no_value_(no_value)
        , min_rebuild_bound_(min_rebuild_bound)
        , rebuild_factor_(rebuild_factor)
        , root_(new NODE(min_rebuild_bound_)) {
        Check(min_rebuild_bound_ > 0);
        Check(rebuild_factor_ > 1);
    }

    explicit SABT(const V& no_value)
        : SABT(no_value, kMinRebuildBound, kRebuildFactor) {
    }

    ~SABT() {
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
                if (node->keys_size < kMaxKeys) {
                    node->Insert(index, key, value, 1);
                } else {
                    NODE* child = new NODE(min_rebuild_bound_);
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
        Validate(root_, nullptr, nullptr);
    }

    NODE* GetRoot() {
        return root_;
    }

    NodeHandler* GetNodeHandler() {
        return new NodeHandler();
    }

private:
    int64_t GetBound(int64_t total_accesses) {
        return ceil(total_accesses / static_cast<double>(kMaxKeys + 1));
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

        NODE* result = BuildIdealTree(0, data.size(), data, psum_accesses);

        delete node;

        return result;
    }

    static void CollectNonErased(NODE* node, std::vector<std::tuple<K, V, int>>& data) {
        assert(node);

        if (node->children && node->children[0]) {
            CollectNonErased(node->children[0], data);
        }

        for (int index = 0; index < node->keys_size; ++index) {
            if (node->values[index].second > 0) {
                data.emplace_back(std::move(node->keys[index]),
                                  std::move(node->values[index].first), node->values[index].second);
            }
            if (node->children && node->children[index + 1]) {
                CollectNonErased(node->children[index + 1], data);
            }
        }
    }

    // [l, r)
    NODE* BuildIdealTree(int left, int right, std::vector<std::tuple<K, V, int>>& data,
                         std::vector<int64_t>& psum_accesses) {
        const int size = right - left;
        if (size == 0) {
            return nullptr;
        }

        const int64_t total_accesses = psum_accesses[right] - psum_accesses[left];
        const int64_t rebuild_bound =
            std::max(min_rebuild_bound_, static_cast<int64_t>(rebuild_factor_ * total_accesses));

        NODE* node = new NODE(rebuild_bound);
        node->total_asize = size;
        node->total_accesses = total_accesses;

        if (size <= kMaxKeys) {
            node->keys_size = size;
            for (int index = 0; index < size; ++index) {
                node->keys[index] = std::move(std::get<0>(data[left + index]));
                node->values[index].first = std::move(std::get<1>(data[left + index]));
                node->values[index].second = std::get<2>(data[left + index]);
            }
            return node;
        }

        const int64_t bound = GetBound(total_accesses);

        node->keys_size = kMaxKeys;

        for (int index = 0; index < kMaxKeys; ++index) {
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
            l = std::min(right - kMaxKeys + index, l);  // to prevent overflow
            node->keys[index] = std::move(std::get<0>(data[l]));
            node->values[index].first = std::move(std::get<1>(data[l]));
            node->values[index].second = std::get<2>(data[l]);
            node->children[index] = BuildIdealTree(left, l, data, psum_accesses);
            left = l + 1;
        }
        node->children[kMaxKeys] = BuildIdealTree(left, right, data, psum_accesses);

        return node;
    }

    std::pair<int, int64_t> Validate(const NODE* node, const K* left_key_bound,
                                     const K* right_key_bound) const {
        if (!node) {
            return {0, 0};
        }

        Check(node->keys_size <= kMaxKeys);

        int total_size = 0;
        int64_t total_accesses = 0;

        for (int index = 0; index <= node->keys_size; ++index) {
            const K* left_key = index == 0 ? left_key_bound : &node->keys[index - 1];
            const K* right_key = index == node->keys_size ? right_key_bound : &node->keys[index];

            CheckIfTrue(left_key && right_key, *left_key < *right_key);

            if (index < node->keys_size) {
                Check(node->values[index].first != no_value_);
                total_size += node->values[index].second > 0;
                total_accesses += std::abs(node->values[index].second);
            }

            auto [child_total_size, child_total_accesses] =
                Validate(node->children[index], left_key, right_key);
            total_size += child_total_size;
            total_accesses += child_total_accesses;
        }

        Check(total_size <= node->total_asize);
        Check(total_accesses <= node->total_accesses);

        return {total_size, total_accesses};
    }
};
