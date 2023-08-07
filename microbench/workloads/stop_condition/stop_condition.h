//
// Created by Ravil Galiev on 21.07.2023.
//

#ifndef SETBENCH_STOP_CONDITION_H
#define SETBENCH_STOP_CONDITION_H

#include "json/single_include/nlohmann/json.hpp"

struct StopCondition {
    virtual void start(size_t numThreads) = 0;

    virtual bool isStopped(int id) = 0;

    virtual std::string toString(size_t indents = 1) = 0;

    virtual void toJson(nlohmann::json &j) const = 0;

    virtual void fromJson(const nlohmann::json &j) = 0;

    virtual ~StopCondition() = default;
};

void to_json(nlohmann::json &j, const StopCondition &s) {
    s.toJson(j);
    assert(j["stopConditionType"] != nullptr);
}

void from_json(const nlohmann::json &j, StopCondition &s) {
    s.fromJson(j);
}

enum class StopConditionType {
    TIMER, OPERATION_COUNTER
};

#endif //SETBENCH_STOP_CONDITION_H
