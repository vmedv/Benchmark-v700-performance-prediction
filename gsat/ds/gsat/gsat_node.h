#pragma once

#include <algorithm>
#include <cstdint>

#include "../../common/error.h"

template<typename Key, typename Value>
struct GSATNode {
public:
    struct ValueData {
        Value value;
        int64_t accesses;
    };

    const Key left;   // inclusive
    const Key right;  // exclusive

    int rep_size;
    int total_asize;  // approximate size (total_asize >= total_size)

    const int64_t rebuild_bound;
    int64_t counter;

    GSATNode(const Key left, const Key right, int64_t rebuild_bound)
            : left(left), right(right), rep_size(0), total_asize(0), rebuild_bound(rebuild_bound), counter(0) {}
};

#define GSAT_NODE_DEFAULT_METHODS                                                       \
int Search(const Key &key) const {                                                      \
    return Search(key, -1, this->rep_size);                                             \
}                                                                                       \
inline bool KeyFound(int index, const Key &key_to_find) const {                         \
    return index != this->rep_size && this->rep[index] == key_to_find;                  \
}                                                                                       \
bool Access() {                                                                         \
    ++this->counter;                                                                    \
    return this->counter > this->rebuild_bound;                                         \
}                                                                                       \
void Insert(int index, const Key &key, const Value &value, int64_t accesses) {          \
    for (int at = this->rep_size - 1; at >= index; --at) {                              \
        this->rep[at + 1] = std::move(rep[at]);                                         \
        this->value_data[at + 1] = std::move(value_data[at]);                           \
    }                                                                                   \
    this->rep[index] = key;                                                             \
    this->value_data[index] = {value, accesses};                                        \
    ++this->rep_size;                                                                   \
}                                                                                       \
