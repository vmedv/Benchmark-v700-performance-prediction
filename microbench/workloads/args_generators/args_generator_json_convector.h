//
// Created by Ravil Galiev on 27.07.2023.
//

#ifndef SETBENCH_ARGS_GENERATOR_JSON_CONVECTOR_H
#define SETBENCH_ARGS_GENERATOR_JSON_CONVECTOR_H


#include "json/single_include/nlohmann/json.hpp"
#include "args_generator_builder.h"
#include "workloads/args_generators/impls/default_args_generator.h"

ArgsGeneratorBuilder *getArgsGeneratorFromJson(const nlohmann::json &j) {
    ArgsGeneratorType type = j["argsGeneratorType"];
    ArgsGeneratorBuilder * argsGeneratorBuilder;
    switch (type) {
        case ArgsGeneratorType::DEFAULT:
            argsGeneratorBuilder = new DefaultArgsGeneratorBuilder();
            break;
        case ArgsGeneratorType::SKEWED_SETS:
            break;
        case ArgsGeneratorType::TEMPORARY_SKEWED:
            break;
        case ArgsGeneratorType::CREAKERS_AND_WAVE:
            break;
        case ArgsGeneratorType::OPS:
            break;
        case ArgsGeneratorType::TEMPORARY_WAVE_SPECIAL_REMOVE:
            break;
    }
    argsGeneratorBuilder->fromJson(j);
    return argsGeneratorBuilder;
}

#endif //SETBENCH_ARGS_GENERATOR_JSON_CONVECTOR_H
