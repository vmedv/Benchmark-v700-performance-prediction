#pragma once

#include <algorithm>
#include <cstring>

#ifdef KEY_SEARCH_TOTAL_STAT
extern int64_t key_search_total_iters_cnt__;
extern int64_t key_search_total_cnt__;
#endif

template <typename K, typename V, int kMaxKeys>
struct BTNode {
    static_assert(kMaxKeys > 1, "kMaxKeys must be > 1");

    K keys[kMaxKeys];
    V values[kMaxKeys];
    BTNode* children[kMaxKeys + 1];
    int size;

    void InitializeDefault() {
        std::memset(children, 0, sizeof(children));
        size = 0;
    }

    void InitializeMove(K* keys, V* values, BTNode** children, int size) {
        std::memset(this->children, 0, sizeof(this->children));
        std::move(keys, keys + size, this->keys);
        std::move(values, values + size, this->values);
        std::copy(children, children + size + 1, this->children);
        this->size = size;
    }

    int FindKey(const K& key) const {
        int l = -1;
        int r = size;
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
        return index != size && keys[index] == key_to_find;
    }
};
