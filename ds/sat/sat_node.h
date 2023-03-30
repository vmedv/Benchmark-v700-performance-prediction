#pragma once

#include <algorithm>
#include <cstdint>

template <typename K, typename V>
struct SATNode {
    K key;
    V value;
    int accesses;

    int total_asize;  // approximate size (total_asize >= total_size)

    int64_t total_accesses;
    int64_t rebuild_bound;

    SATNode* left = nullptr;
    SATNode* right = nullptr;

    SATNode(const K& key, const V& value)
        : key(key)
        , value(value) {
    }

    SATNode(K&& key, V&& value)
        : key(std::move(key))
        , value(std::move(value)) {
    }

    ~SATNode() {
        delete left;
        delete right;
    }

    bool Access() {
        ++total_accesses;
        return total_accesses > rebuild_bound;
    }
};
