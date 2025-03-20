#pragma once

#include <complex>
#include "workloads/thread_loops/thread_loop.h"
#include "workloads/args_generators/args_generator.h"
#include "workloads/thread_loops/ratio_thread_loop_parameters.h"

//template<typename K>
class SeqThreadLoop : public ThreadLoop {

    ArgsGenerator<K> *argsGenerator;
    int state = 0;

public:
    SeqThreadLoop(globals_t *_g, size_t _threadId, StopCondition *_stopCondition, size_t _RQ_RANGE, ArgsGenerator<K> *_argsGenerator)
            : ThreadLoop(_g, _threadId, _stopCondition, _RQ_RANGE),
              argsGenerator(_argsGenerator) {}

    void step() override {
	int step = state++;
	if (step % 4 == 0) {
		auto k = argsGenerator->nextInsert();
		this->executeInsert(k);
	} else if (step % 4 == 1) {
		auto k = argsGenerator->nextRemove();
		this->executeRemove(k);
	} else if (step % 4 == 2) {
		/* auto [l, r] = argsGenerator->nextRange(); */
		/* this->executeRangeQuery(l, r); */
	} else if (step % 4 == 3) {
		auto k = argsGenerator->nextGet();
		this->GET_FUNC(k);
	} else {
		assert(false);
	}
    }
};

#include "workloads/thread_loops/thread_loop_builder.h"
#include "workloads/args_generators/args_generator_builder.h"
#include "workloads/args_generators/impls/default_args_generator.h"
#include "workloads/args_generators/args_generator_json_convector.h"
#include "globals_extern.h"

//template<typename K>
struct SeqThreadLoopBuilder : public ThreadLoopBuilder {
    ArgsGeneratorBuilder *argsGeneratorBuilder = new SeqArgGeneratorBuilder();

    SeqThreadLoopBuilder *init(int range) override {
        ThreadLoopBuilder::init(range);
        argsGeneratorBuilder->init(range);
        return this;
    }

//    template<typename K>
    ThreadLoop *build(globals_t *_g, Random64 &_rng, size_t _threadId, StopCondition *_stopCondition) override {
        return new SeqThreadLoop(_g, _threadId, _stopCondition, this->RQ_RANGE,
                                     argsGeneratorBuilder->build(_rng));
    }

    void toJson(nlohmann::json &json) const override {
        json["ClassName"] = "SeqThreadLoopBuilder";
        json["argsGeneratorBuilder"] = *argsGeneratorBuilder;
    }


    void fromJson(const nlohmann::json &j) override {
        argsGeneratorBuilder = getArgsGeneratorFromJson(j["argsGeneratorBuilder"]);
    }

    std::string toString(size_t indents = 1) override {
	    return {};
    }

    ~SeqThreadLoopBuilder() override {
        delete argsGeneratorBuilder;
    };
};


