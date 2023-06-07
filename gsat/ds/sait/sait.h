#pragma once

#include "../gsat/gsat.h"

#include "sqrt_delimiter.h"
#include "sait_node.h"

template<typename Key, typename Value>
using SAITBase = GSAT<SqrtDelimiter, SAITNode<Key, Value>, Key, Value>;

template<typename Key, typename Value>
class SAIT : public SAITBase<Key, Value> {
public:
    using Node = SAITNode<Key, Value>;

    static constexpr int kMinLeafSize = 8;
    static constexpr int64_t kMinRebuildBound = 125;
    static constexpr double kRebuildFactor = 0.25;

    SAIT(const Value &no_value, const Key &left, const Key &right, int leaf_size, int64_t min_rebuild_bound,
         double rebuild_factor)
            : SAITBase<Key, Value>(no_value, left, right, leaf_size, min_rebuild_bound, rebuild_factor) {}

    SAIT(const Value &no_value, const Key &left, const Key &right)
            : SAIT(no_value, left, right, kMinLeafSize, kMinRebuildBound, kRebuildFactor) {}
};
