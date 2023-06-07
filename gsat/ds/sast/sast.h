#pragma once

#include "../gsat/gsat.h"

#include "../sabt/constant_delimiter.h"
#include "sast_node.h"

template<typename Key, typename Value>
using SASTBase = GSAT<ConstantDelimiter<1>, SASTNode<Key, Value>, Key, Value>;

template<typename Key, typename Value>
class SAST : public SASTBase<Key, Value> {
public:
    static constexpr int64_t kMinRebuildBound = 125;
    static constexpr double kRebuildFactor = 0.25;

    using Node = SASTNode<Key, Value>;
    using Base = SASTBase<Key, Value>;
    using NodeHandler = typename Base::NodeHandler;

    SAST(const Value &no_value, const Key &left, const Key &right, int64_t min_rebuild_bound, double rebuild_factor)
            : Base(no_value, left, right, 1, min_rebuild_bound, rebuild_factor) {}

    SAST(const Value &no_value, const Key &left, const Key &right)
            : SAST(no_value, left, right, kMinRebuildBound, kRebuildFactor) {}
};
