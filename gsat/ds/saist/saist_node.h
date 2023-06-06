#pragma once

#include <algorithm>
#include <cstdint>

#include "error.h"

#ifdef KEY_SEARCH_TOTAL_STAT
extern int64_t key_search_total_iters_cnt__;
extern int64_t key_search_total_cnt__;
#endif


template <typename K, typename V>
struct SAISTNode {
    using Value = std::pair<V, int>;

    K* keys;
    Value* values;
    int* ids;
    SAISTNode** children;  // if node is leaf then children == nullptr and ids == nullptr

    const K& left_bound;   // inclusive
    const K& right_bound;  // exclusive

    int keys_size;
    int ids_size;

    int total_asize;  // approximate size (total_asize >= total_size)
    int capacity;

    const int64_t rebuild_bound;
    int64_t total_accesses;

    SAISTNode(const K& left_bound, const K& right_bound, int capacity, int64_t rebuild_bound)
        : keys(new K[capacity])
        , values(new Value[capacity])
        , ids(nullptr)
        , children(new SAISTNode*[capacity + 1])
        , left_bound(left_bound)
        , right_bound(right_bound)
        , keys_size(0)
        , ids_size(0)
        , total_asize(0)
        , capacity(capacity)
        , rebuild_bound(rebuild_bound)
        , total_accesses(0) {
        std::fill(children, children + capacity + 1, nullptr);
    }

    ~SAISTNode() {
        delete[] keys;
        delete[] values;
        delete[] ids;

        for (int index = 0; index <= capacity; ++index) {
            delete children[index];
        }
        delete[] children;
    }

    void InitializeIds(int ids_size) {
        this->ids_size = ids_size;
        ids = new int[ids_size];

        int j = 0;
        for (int i = 0; i < ids_size; ++i) {
            K cur =
                left_bound + (i + 1) * (static_cast<int64_t>(right_bound) - left_bound) / ids_size;
            while (j < keys_size && keys[j] < cur) {
                ++j;
            }
            ids[i] = j;
        }
    }

    int FindKey(const K& key) const {
        int l = -1;
        int r = keys_size;

        if (ids) {
#ifdef KEY_SEARCH_TOTAL_STAT
            ++key_search_total_iters_cnt__;
#endif
            int j = (static_cast<int64_t>(key) - left_bound) * ids_size /
                    (static_cast<int64_t>(right_bound) - left_bound);
            r = ids[j];
            l = j == 0 ? -1 : ids[j - 1] - 1;
        }

        while (r - l > 1) {
#ifdef KEY_SEARCH_TOTAL_STAT
            ++key_search_total_iters_cnt__;
#endif
            int m = (l + r) >> 1;
            if (keys[m] < key) {
                l = m;
            } else {
                r = m;
            }
        }
#ifdef KEY_SEARCH_TOTAL_STAT
        ++key_search_total_cnt__;
#endif
        return r;
    }

    inline bool IsKeyFound(int index, const K& key_to_find) const {
        return index != keys_size && keys[index] == key_to_find;
    }

    bool Access() {
        ++total_accesses;
        return total_accesses > rebuild_bound;
    }

    void Insert(int index, const K& key, const V& value, int accesses) {
        for (int at = keys_size - 1; at >= index; --at) {
            keys[at + 1] = std::move(keys[at]);
            values[at + 1] = std::move(values[at]);
        }

        keys[index] = key;
        values[index].first = value;
        values[index].second = accesses;

        ++keys_size;
    }
};
