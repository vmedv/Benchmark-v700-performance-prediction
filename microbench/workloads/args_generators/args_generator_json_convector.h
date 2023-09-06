//
// Created by Ravil Galiev on 27.07.2023.
//

#ifndef SETBENCH_ARGS_GENERATOR_JSON_CONVECTOR_H
#define SETBENCH_ARGS_GENERATOR_JSON_CONVECTOR_H


#include "json/single_include/nlohmann/json.hpp"
#include "args_generator_builder.h"
#include "workloads/args_generators/impls/default_args_generator.h"
#include "workloads/args_generators/impls/skewed_sets_args_generator.h"
#include "workloads/args_generators/impls/creakers_and_wave_args_generator.h"
#include "workloads/args_generators/impls/temporary_skewed_args_generator.h"

ArgsGeneratorBuilder *getArgsGeneratorFromJson(const nlohmann::json &j) {
    ArgsGeneratorType type = j["argsGeneratorType"];
    ArgsGeneratorBuilder * argsGeneratorBuilder;
    switch (type) {
        case ArgsGeneratorType::DEFAULT:
            argsGeneratorBuilder = new DefaultArgsGeneratorBuilder();
            break;
        case ArgsGeneratorType::SKEWED_SETS:
            argsGeneratorBuilder = new SkewedSetsArgsGeneratorBuilder();
            break;
        case ArgsGeneratorType::TEMPORARY_SKEWED:
            argsGeneratorBuilder = new TemporarySkewedArgsGeneratorBuilder();
            break;
        case ArgsGeneratorType::CREAKERS_AND_WAVE:
            argsGeneratorBuilder = new CreakersAndWaveArgsGeneratorBuilder();
            break;
        case ArgsGeneratorType::CREAKERS_AND_WAVE_PREFILL:
            argsGeneratorBuilder = new CreakersAndWavePrefillArgsGeneratorBuilder();
            break;
    }
    argsGeneratorBuilder->fromJson(j);
    return argsGeneratorBuilder;
}

#endif //SETBENCH_ARGS_GENERATOR_JSON_CONVECTOR_H
