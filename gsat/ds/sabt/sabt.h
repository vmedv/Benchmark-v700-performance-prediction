#pragma once

#include "../gsat/gsat.h"

#include "constant_delimiter.h"
#include "sabt_node.h"

template<typename Key, typename Value, int kMaxKeys>
using SABTBase = GSAT<ConstantDelimiter<kMaxKeys>, SABTNode<Key, Value, kMaxKeys>, Key, Value>;

template<typename Key, typename Value, int kMinKeys>
class SABT : public SABTBase<Key, Value, 2 * kMinKeys> {
public:
    static_assert(kMinKeys > 0, "min keys must be greater zero");

    static constexpr int kMaxKeys = 2 * kMinKeys;

    static constexpr int64_t kMinRebuildBound = 125;
    static constexpr double kRebuildFactor = 0.25;

    using Node = SABTNode<Key, Value, kMaxKeys>;

    SABT(const Value &no_value, const Key &left, const Key &right, int64_t min_rebuild_bound, double rebuild_factor)
            : SABTBase<Key, Value, kMaxKeys>(no_value, left, right, kMaxKeys, min_rebuild_bound, rebuild_factor) {}

    SABT(const Value &no_value, const Key &left, const Key &right)
            : SABT(no_value, left, right, kMinRebuildBound, kRebuildFactor) {}
};
