//
// Created by Ravil Galiev on 07.08.2023.
//

#ifndef SETBENCH_PREFILL_INSERT_THREAD_LOOP_H
#define SETBENCH_PREFILL_INSERT_THREAD_LOOP_H

#include <string>

//typedef long long K;

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

public:
    PrefillInsertThreadLoop(globals_t *_g, Random64 &_rng, size_t _threadId,
                            StopCondition *_stopCondition, size_t _RQ_RANGE,
                            ArgsGenerator<K> *_argsGenerator)
            : ThreadLoop(_g, _threadId, _stopCondition, _RQ_RANGE),
              rng(_rng), argsGenerator(_argsGenerator) {
    }

    void step() override {
        K *value = (K *) this->NO_VALUE;
        while (value == (K *) this->NO_VALUE) {
            K key = this->argsGenerator->nextInsert();
            value = this->executeInsert(key);
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
                                                  argsGeneratorBuilder->build(_rng));
    }

    void toJson(nlohmann::json &json) const override {
        json["threadLoopType"] = ThreadLoopType::PREFILL_INSERT;
//        json["insFrac"] = parameters->INS_FRAC;
//        json["delFrac"] = parameters->DEL_FRAC;
//        json["rqFrac"] = parameters->RQ;
        json["argsGeneratorBuilder"] = *argsGeneratorBuilder;
    }


    void fromJson(const nlohmann::json &j) override {
//        parameters->INS_FRAC = j["insFrac"];
//        parameters->DEL_FRAC = j["delFrac"];
//        parameters->RQ = j["rqFrac"];
        argsGeneratorBuilder = getArgsGeneratorFromJson(j["argsGeneratorBuilder"]);
    }

    std::string toString(size_t indents = 1) override {
        return indented_title_with_str_data("Type", "Prefill Insert", indents)
               //               + indented_title_with_data("INS_FRAC", parameters->INS_FRAC, indents)
               //               + indented_title_with_data("DEL_FRAC", parameters->DEL_FRAC, indents)
               //               + indented_title_with_data("RQ", parameters->RQ, indents)
               + indented_title("Args generator", indents)
               + argsGeneratorBuilder->toString(indents + 1);
    }

    ~PrefillInsertThreadLoopBuilder() override = default;
};

#endif //SETBENCH_PREFILL_INSERT_THREAD_LOOP_H
