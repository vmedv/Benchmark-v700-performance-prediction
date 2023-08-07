//
// Created by Ravil Galiev on 21.07.2023.
//

#ifndef SETBENCH_THREAD_LOOP_BUILDER_H
#define SETBENCH_THREAD_LOOP_BUILDER_H

#include <string>
#include "thread_loop.h"
#include "json/single_include/nlohmann/json.hpp"
#include "globals_t.h"

enum class ThreadLoopType {
    DEFAULT, TEMPORARY_OPERATION, PREFILL_INSERT
};

//template<typename K>
struct ThreadLoopBuilder {
    size_t RQ_RANGE;

    virtual ThreadLoopBuilder *init(int range) {
        RQ_RANGE = range;
        return this;
    };

//    template<class GlobalsT>
    virtual ThreadLoop *build(globals_t *_g, Random64 & _rng, size_t _tid, StopCondition *_stopCondition) = 0;

    virtual void toJson(nlohmann::json &j) const = 0;

    virtual void fromJson(const nlohmann::json &j) = 0;

//    static void to_json(nlohmann::json &j, const ThreadLoopBuilder &s);

//    static void from_json(const nlohmann::json &j, ThreadLoopBuilder &s);

    virtual std::string toString(size_t indents = 1) = 0;

    virtual ~ThreadLoopBuilder() = default;

//    virtual toJson()
};


void to_json(nlohmann::json &j, const ThreadLoopBuilder &s) {
    s.toJson(j);
    assert(j["threadLoopType"] != nullptr);
}

void from_json(const nlohmann::json &j, ThreadLoopBuilder &s) {
    s.fromJson(j);
}

//ThreadLoopBuilder & from_json(const nlohmann::json &j, )

#endif //SETBENCH_THREAD_LOOP_BUILDER_H
