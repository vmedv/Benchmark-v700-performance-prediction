#pragma once

#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <utility>

#ifdef KEY_SEARCH_TOTAL_STAT
extern int64_t key_search_total_iters_cnt__;
extern int64_t key_search_total_cnt__;
#endif

template <typename K, typename V, int kMaxKeys>
struct SABTNode {
    static_assert(kMaxKeys > 1, "MaxKeys must be > 1");

    using Value = std::pair<V, int>;

    K keys[kMaxKeys];
    Value values[kMaxKeys];  // accesses < 0 => key is deleted
    SABTNode* children[kMaxKeys + 1];

    int keys_size;
    int total_asize;

    const int64_t rebuild_bound;
    int64_t total_accesses;

    explicit SABTNode(int64_t rebuild_bound)
        : keys_size(0)
        , total_asize(0)
        , rebuild_bound(rebuild_bound)
        , total_accesses(0) {
        std::memset(children, 0, sizeof(children));
    }

    ~SABTNode() {
        for (int index = 0; index <= kMaxKeys; ++index) {
            delete children[index];
        }
    }

    int FindKey(const K& key) const {
        int l = -1;
        int r = keys_size;
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
        assert(keys_size + 1 <= kMaxKeys);

        for (int at = keys_size - 1; at >= index; --at) {
            keys[at + 1] = std::move(keys[at]);
            values[at + 1] = std::move(values[at]);
        }

        keys[index] = key;
        values[index] = {value, accesses};

        ++keys_size;
    }
};
