//
// Created by Ravil Galiev on 06.04.2023.
//

#pragma once

#include "workloads/thread_loops/thread_loop.h"
#include "workloads/args_generators/args_generator.h"
#include "workloads/thread_loops/ratio_thread_loop_parameters.h"

// template<typename K>
class GetThreadLoop : public ThreadLoop {
    ArgsGenerator<K>* argsGenerator;

public:
    GetThreadLoop(globals_t* _g, Random64& _rng, size_t _threadId,
                      StopCondition* _stopCondition, size_t _RQ_RANGE,
                      ArgsGenerator<K>* _argsGenerator,
                      RatioThreadLoopParameters& threadLoopParameters)
        : ThreadLoop(_g, _threadId, _stopCondition, _RQ_RANGE),
          argsGenerator(_argsGenerator) {
    }

    void step() override {
        K key = this->argsGenerator->nextGet();
        this->GET_FUNC(key);
    }
};

#include "workloads/thread_loops/thread_loop_builder.h"
#include "workloads/args_generators/args_generator_builder.h"
#include "workloads/args_generators/impls/default_args_generator.h"
#include "workloads/args_generators/args_generator_json_convector.h"
#include "globals_extern.h"

// template<typename K>
struct GetThreadLoopBuilder : public ThreadLoopBuilder {
    RatioThreadLoopParameters parameters;

    ArgsGeneratorBuilder* argsGeneratorBuilder = new DefaultArgsGeneratorBuilder();

    GetThreadLoopBuilder* setArgsGeneratorBuilder(ArgsGeneratorBuilder* _argsGeneratorBuilder) {
        argsGeneratorBuilder = _argsGeneratorBuilder;
        return this;
    }

    GetThreadLoopBuilder* init(int range) override {
        ThreadLoopBuilder::init(range);
        argsGeneratorBuilder->init(range);
        return this;
    }

    //    template<typename K>
    ThreadLoop* build(globals_t* _g, Random64& _rng, size_t _threadId,
                      StopCondition* _stopCondition) override {
        return new GetThreadLoop(_g, _rng, _threadId, _stopCondition, this->RQ_RANGE,
                                     argsGeneratorBuilder->build(_rng), parameters);
    }

    void toJson(nlohmann::json& json) const override {
        json["ClassName"] = "GetThreadLoopBuilder";
        json["argsGeneratorBuilder"] = *argsGeneratorBuilder;
    }

    void fromJson(const nlohmann::json& j) override {
        argsGeneratorBuilder = getArgsGeneratorFromJson(j["argsGeneratorBuilder"]);
    }

    std::string toString(size_t indents = 1) override {
        return indented_title_with_str_data("Type", "Get", indents) +
               indented_title("Args generator", indents) +
               argsGeneratorBuilder->toString(indents + 1);
    }

    ~GetThreadLoopBuilder() override {
        delete argsGeneratorBuilder;
    };
};

