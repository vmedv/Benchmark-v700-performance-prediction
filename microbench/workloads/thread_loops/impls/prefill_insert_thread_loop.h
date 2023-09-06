//
// Created by Ravil Galiev on 07.08.2023.
//

#ifndef SETBENCH_PREFILL_INSERT_THREAD_LOOP_H
#define SETBENCH_PREFILL_INSERT_THREAD_LOOP_H

#include <string>

#include "workloads/thread_loops/thread_loop.h"
#include "workloads/args_generators/args_generator.h"

//template<typename K>
class PrefillInsertThreadLoop : public ThreadLoop {
private:
    PAD;
    Random64 &rng;
    PAD;
    ArgsGenerator<K> *argsGenerator;
    PAD;
    size_t number_of_attempts;

public:
    PrefillInsertThreadLoop(globals_t *_g, Random64 &_rng, size_t _threadId,
                            StopCondition *_stopCondition, size_t _RQ_RANGE,
                            ArgsGenerator<K> *_argsGenerator, size_t _number_of_attempts)
            : ThreadLoop(_g, _threadId, _stopCondition, _RQ_RANGE),
              rng(_rng), argsGenerator(_argsGenerator), number_of_attempts(_number_of_attempts) {
    }

    void step() override {
        size_t counter = 0;
        K *value;
        do {
            K key = this->argsGenerator->nextInsert();
            value = this->executeInsert(key);
            ++counter;
        } while (value != (K *) this->NO_VALUE && counter < number_of_attempts);

        if (value != (K *) this->NO_VALUE) {
            COUTATOMICTID("WARNING: PrefillInsertThreadLoop have not inserted a new key. Number of attempts is: "
                                  << number_of_attempts << "\n")
        }
    }
};

#include "workloads/thread_loops/thread_loop_builder.h"
#include "workloads/args_generators/args_generator_builder.h"
#include "workloads/args_generators/impls/default_args_generator.h"
#include "workloads/args_generators/args_generator_json_convector.h"
#include "globals_extern.h"

//template<typename K>
struct PrefillInsertThreadLoopBuilder : public ThreadLoopBuilder {
    ArgsGeneratorBuilder *argsGeneratorBuilder = new DefaultArgsGeneratorBuilder();
    size_t number_of_attempts = 10e+6;

    PrefillInsertThreadLoopBuilder *setNumberOfAttempts(size_t _numberOfAttempts) {
        number_of_attempts = _numberOfAttempts;
        return this;
    }

    PrefillInsertThreadLoopBuilder *setArgsGeneratorBuilder(ArgsGeneratorBuilder *_argsGeneratorBuilder) {
        argsGeneratorBuilder = _argsGeneratorBuilder;
        return this;
    }

    PrefillInsertThreadLoopBuilder *init(int range) override {
        ThreadLoopBuilder::init(range);
        argsGeneratorBuilder->init(range);
        return this;
    }

//    template<typename K>
    ThreadLoop *build(globals_t *_g, Random64 &_rng, size_t _threadId, StopCondition *_stopCondition) override {
        return new PrefillInsertThreadLoop(_g, _rng, _threadId, _stopCondition, this->RQ_RANGE,
                                           argsGeneratorBuilder->build(_rng), number_of_attempts);
    }

    void toJson(nlohmann::json &json) const override {
        json["threadLoopType"] = ThreadLoopType::PREFILL_INSERT;
        json["number_of_attempts"] = number_of_attempts;
        json["argsGeneratorBuilder"] = *argsGeneratorBuilder;
    }

    void fromJson(const nlohmann::json &j) override {
        if (j.contains("number_of_attempts")) {
            number_of_attempts = j["number_of_attempts"];
        }
        argsGeneratorBuilder = getArgsGeneratorFromJson(j["argsGeneratorBuilder"]);
    }

    std::string toString(size_t indents = 1) override {
        return indented_title_with_str_data("Type", "Prefill Insert", indents)
               + indented_title_with_data("Number of attempts", number_of_attempts, indents)
               + indented_title("Args generator", indents)
               + argsGeneratorBuilder->toString(indents + 1);
    }

    ~PrefillInsertThreadLoopBuilder() override {
        delete argsGeneratorBuilder;
    };
};

#endif //SETBENCH_PREFILL_INSERT_THREAD_LOOP_H
