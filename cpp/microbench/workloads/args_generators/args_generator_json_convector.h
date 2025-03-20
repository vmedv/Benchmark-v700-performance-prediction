//
// Created by Ravil Galiev on 27.07.2023.
//

#ifndef SETBENCH_ARGS_GENERATOR_JSON_CONVECTOR_H
#define SETBENCH_ARGS_GENERATOR_JSON_CONVECTOR_H


#include "json/single_include/nlohmann/json.hpp"
#include "args_generator_builder.h"
#include "workloads/args_generators/impls/default_args_generator.h"
#include "workloads/args_generators/impls/skewed_sets_args_generator.h"
#include "workloads/args_generators/impls/skewed_insert_args_generator.h"
#include "workloads/args_generators/impls/creakers_and_wave_args_generator.h"
#include "workloads/args_generators/impls/temporary_skewed_args_generator.h"
#include "workloads/args_generators/impls/leafs_handshake_args_generator.h"
#include "workloads/args_generators/impls/seq_arg_generator.h"
#include "errors.h"

ArgsGeneratorBuilder *getArgsGeneratorFromJson(const nlohmann::json &j) {
    std::string className = j["ClassName"];
    ArgsGeneratorBuilder *argsGeneratorBuilder;
    if (className == "DefaultArgsGeneratorBuilder") {
        argsGeneratorBuilder = new DefaultArgsGeneratorBuilder();
    } else if (className == "SkewedSetsArgsGeneratorBuilder") {
        argsGeneratorBuilder = new SkewedSetsArgsGeneratorBuilder();
    } else if (className == "TemporarySkewedArgsGeneratorBuilder") {
        argsGeneratorBuilder = new TemporarySkewedArgsGeneratorBuilder();
    } else if (className == "CreakersAndWaveArgsGeneratorBuilder") {
        argsGeneratorBuilder = new CreakersAndWaveArgsGeneratorBuilder();
    } else if (className == "CreakersAndWavePrefillArgsGeneratorBuilder") {
        argsGeneratorBuilder = new CreakersAndWavePrefillArgsGeneratorBuilder();
    } else if (className == "LeafsHandshakeArgsGeneratorBuilder") {
        argsGeneratorBuilder = new LeafsHandshakeArgsGeneratorBuilder();
    } else if (className == "SkewedInsertArgsGeneratorBuilder") {
        argsGeneratorBuilder = new SkewedInsertArgsGeneratorBuilder();
    } else if (className == "SeqArgsGeneratorBuilder") {
	argsGeneratorBuilder = new SeqArgGeneratorBuilder();
    } else {
        setbench_error("JSON PARSER: Unknown class name ArgsGeneratorBuilder -- " + className)
    }

    argsGeneratorBuilder->fromJson(j);
    return argsGeneratorBuilder;
}

#endif //SETBENCH_ARGS_GENERATOR_JSON_CONVECTOR_H
