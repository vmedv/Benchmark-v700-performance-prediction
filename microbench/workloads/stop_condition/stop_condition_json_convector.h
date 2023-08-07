//
// Created by Ravil Galiev on 31.07.2023.
//

#ifndef SETBENCH_STOP_CONDITION_JSON_CONVECTOR_H
#define SETBENCH_STOP_CONDITION_JSON_CONVECTOR_H

#include "json/single_include/nlohmann/json.hpp"
#include "stop_condition.h"
#include "workloads/stop_condition/impls/timer.h"
#include "workloads/stop_condition/impls/operation_counter.h"

StopCondition *getStopConditionFromJson(const nlohmann::json &j) {
    StopConditionType type = j["stopConditionType"];
    StopCondition * stopCondition;
    switch (type) {
        case StopConditionType::TIMER:
            stopCondition = new Timer();
            break;
        case StopConditionType::OPERATION_COUNTER:
            stopCondition = new OperationCounter();
            break;
    }
    stopCondition->fromJson(j);
    return stopCondition;
}

#endif //SETBENCH_STOP_CONDITION_JSON_CONVECTOR_H
