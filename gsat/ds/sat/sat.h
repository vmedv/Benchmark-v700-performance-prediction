#pragma once

#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <utility>
#include <vector>
#include <tuple>

#include "../../common/error.h"

#include "sat_node.h"
#include "sat_node_handler.h"

#ifdef KEY_DEPTH_TOTAL_STAT
extern int64_t key_depth_total_sum__;
extern int64_t key_depth_total_cnt__;
#endif

#ifdef KEY_DEPTH_STAT
extern int64_t* key_depth_sum__;
extern int64_t* key_depth_cnt__;
#endif

template <typename K, typename V>
class SAT {
public:
    static constexpr int64_t kMinRebuildBound = 100;
    static constexpr double kRebuildFactor = 1.25;

    using NODE = SATNode<K, V>;
    using NodeHandler = SATNodeHandler<K, V>;
    static_assert(std::is_same<NODE*, typename NodeHandler::NodePtrType>::value);

private:
    const V no_value_;
    const int64_t min_rebuild_bound_;
    const double rebuild_factor_;
    NODE* root_;

public:
    SAT(const V& no_value, int64_t min_rebuild_bound, double rebuild_factor)
        : no_value_(no_value)
        , min_rebuild_bound_(min_rebuild_bound)
        , rebuild_factor_(rebuild_factor)
        , root_(nullptr) {
        Check(min_rebuild_bound_ > 1);
        Check(rebuild_factor_ > 1);
    }

    explicit SAT(const V& no_value)
        : SAT(no_value, kMinRebuildBound, kRebuildFactor) {
    }

    ~SAT() {
        delete root_;
    }

    V Find(const K& key) {
        if (!root_) {
            return no_value_;
        }
#if defined KEY_DEPTH_TOTAL_STAT || defined KEY_DEPTH_STAT
        int d__ = 0;
#endif
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

            if (node->key == key) {
#ifdef KEY_DEPTH_TOTAL_STAT
                key_depth_total_sum__ += d__;
                ++key_depth_total_cnt__;
#endif
#ifdef KEY_DEPTH_STAT
                key_depth_sum__[key] += d__;
                ++key_depth_cnt__[key];
#endif
                if (node->accesses < 0) {
                    --node->accesses;
                } else {
                    ++node->accesses;
                    result = node->value;
                }
                break;
            } else if (node->key < key) {
                node_at = &node->right;
                node = node->right;
            } else {
                node_at = &node->left;
                node = node->left;
            }
#if defined KEY_DEPTH_TOTAL_STAT || defined KEY_DEPTH_STAT
            ++d__;
#endif
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
        assert(value != no_value_);

        NODE* node = root_;
        NODE** node_at = &root_;

        NODE* rebuild_node = nullptr;
        NODE** rebuild_node_at = nullptr;

        V result = no_value_;

        while (node) {
            ++node->total_asize;

            if (node->Access() && !rebuild_node) {
                rebuild_node = node;
                rebuild_node_at = node_at;
            }

            if (node->key == key) {
                if (node->accesses < 0) {
                    node->accesses = -node->accesses;
                    node->value = value;
                } else {
                    result = node->value;
                }
                ++node->accesses;
                break;
            } else if (node->key < key) {
                node_at = &node->right;
                node = node->right;
            } else {
                node_at = &node->left;
                node = node->left;
            }
        }

        if (!node) {
            auto new_node = new NODE(key, value);
            new_node->accesses = new_node->total_asize = new_node->total_accesses = 1;
            new_node->rebuild_bound = min_rebuild_bound_;
            *node_at = new_node;
        }

        if (rebuild_node) {
            *rebuild_node_at = RebuildTree(rebuild_node);
        }

        return result;
    }

    V Erase(const K& key) {
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

            if (node->key == key) {
                if (node->accesses > 0) {
                    result = node->value;
                    node->accesses = -node->accesses;
                }
                --node->accesses;
                break;
            } else if (node->key < key) {
                node_at = &node->right;
                node = node->right;
            } else {
                node_at = &node->left;
                node = node->left;
            }
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
        if (node->left) {
            CollectNonErased(node->left, data);
        }
        if (node->accesses > 0) {
            data.emplace_back(std::move(node->key), std::move(node->value), node->accesses);
        }
        if (node->right) {
            CollectNonErased(node->right, data);
        }
    }

    // [left, right)
    NODE* BuildIdealTree(int left, int right, std::vector<std::tuple<K, V, int>>& data,
                         std::vector<int64_t>& psum_accesses) {
        int size = right - left;
        if (size <= 0) {
            return nullptr;
        }

        int64_t total_accesses = psum_accesses[right] - psum_accesses[left];
        int64_t rebuild_bound =
            std::max(min_rebuild_bound_, static_cast<int64_t>(rebuild_factor_ * total_accesses));

        int64_t bound = ceil(total_accesses / 2.0);

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

        auto node = new NODE(std::move(std::get<0>(data[l])), std::move(std::get<1>(data[l])));
        node->accesses = std::get<2>(data[l]);
        node->total_asize = size;
        node->total_accesses = total_accesses;
        node->rebuild_bound = rebuild_bound;
        node->left = BuildIdealTree(left, l, data, psum_accesses);
        node->right = BuildIdealTree(l + 1, right, data, psum_accesses);

        return node;
    }

    std::pair<int, int64_t> Validate(const NODE* node, const K* left, const K* right) const {
        if (!node) {
            return {0, 0};
        }

        CheckIfTrue(left, *left < node->key);
        CheckIfTrue(right, node->key < *right);
        Check(node->value != no_value_);

        int total_size = node->accesses > 0;
        int64_t total_accesses = std::abs(node->accesses);

        auto [left_child_size, left_child_total_accesses] = Validate(node->left, left, &node->key);
        total_size += left_child_size;
        total_accesses += left_child_total_accesses;

        auto [right_child_size, right_child_total_accesses] =
            Validate(node->right, &node->key, right);
        total_size += right_child_size;
        total_accesses += right_child_total_accesses;

        Check(total_size <= node->total_asize);
        Check(total_accesses <= node->total_accesses);

        return {total_size, total_accesses};
    }
};
