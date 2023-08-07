//
// Created by Ravil Galiev on 06.04.2023.
//

#ifndef SETBENCH_DEFAULT_THREAD_LOOP_H
#define SETBENCH_DEFAULT_THREAD_LOOP_H


#include <string>

struct DefaultThreadLoopParameters {
    double INS_FRAC;
    double DEL_FRAC;
    double RQ;

    void setInsFrac(double insFrac) {
        INS_FRAC = insFrac;
    }

    void setDelFrac(double delFrac) {
        DEL_FRAC = delFrac;
    }

    void setRq(double rq) {
        RQ = rq;
    }

    std::string toString() const {
        std::string params;
        params += "INS_FRAC                      : " + std::to_string(INS_FRAC) + "\n";
        params += "DEL_FRAC                      : " + std::to_string(DEL_FRAC) + "\n";
        params += "RQ                            : " + std::to_string(RQ) + "\n";

        return params;
    }
};

typedef long long K;

#include "workloads/thread_loops/thread_loop.h"
#include "workloads/args_generators/args_generator.h"
//#include "globals_t_impl.h"

//template<typename K>
class DefaultThreadLoop : public ThreadLoop {
private:
    PAD;
    double *cdf;
    Random64 &rng;
    PAD;
    ArgsGenerator<K> *argsGenerator;
    PAD;

public:
    DefaultThreadLoop(globals_t *_g, Random64 &_rng, size_t _threadId, StopCondition *_stopCondition, size_t _RQ_RANGE,
                      ArgsGenerator<K> *_argsGenerator,
                      DefaultThreadLoopParameters *threadLoopParameters)
            : ThreadLoop(_g, _threadId, _stopCondition, _RQ_RANGE), rng(_rng), argsGenerator(_argsGenerator) {
        cdf = new double[3];
        cdf[0] = threadLoopParameters->INS_FRAC;
        cdf[1] = cdf[0] + threadLoopParameters->DEL_FRAC;
        cdf[2] = cdf[1] + threadLoopParameters->RQ;
    }

    void step() override {
        double op = (double) rng.next() / (double) rng.max_value;
        if (op < cdf[0]) { // insert
            K key = this->argsGenerator->nextInsert();
            this->executeInsert(key);
        } else if (op < cdf[1]) { // erase
            K key = this->argsGenerator->nextRemove();
            this->executeRemove(key);
        } else if (op < cdf[2]) { // range query
            std::pair<K, K> keys = this->argsGenerator->nextRange();
            this->executeRangeQuery(keys.first, keys.second);
        } else { // read
            K key = this->argsGenerator->nextGet();
            this->executeGet(key);
        }
    }
};

#include "workloads/thread_loops/thread_loop_builder.h"
#include "workloads/args_generators/args_generator_builder.h"
#include "workloads/args_generators/impls/default_args_generator.h"
#include "workloads/args_generators/args_generator_json_convector.h"
#include "globals_extern.h"

//template<typename K>
struct DefaultThreadLoopBuilder : public ThreadLoopBuilder {
    DefaultThreadLoopParameters *parameters = new DefaultThreadLoopParameters();

    ArgsGeneratorBuilder *argsGeneratorBuilder = new DefaultArgsGeneratorBuilder();

    DefaultThreadLoopBuilder *setInsFrac(double insFrac) {
        parameters->INS_FRAC = insFrac;
        return this;
    }

    DefaultThreadLoopBuilder *setRemFrac(double delFrac) {
        parameters->DEL_FRAC = delFrac;
        return this;
    }

    DefaultThreadLoopBuilder *setRq(double rq) {
        parameters->RQ = rq;
        return this;
    }

    DefaultThreadLoopBuilder *setArgsGeneratorBuilder(ArgsGeneratorBuilder *_argsGeneratorBuilder) {
        argsGeneratorBuilder = _argsGeneratorBuilder;
        return this;
    }

    DefaultThreadLoopBuilder *init(int range) override {
        ThreadLoopBuilder::init(range);
        argsGeneratorBuilder->init(range);
        return this;
    }

//    template<typename K>
    ThreadLoop *build(globals_t *_g, Random64 &_rng, size_t _threadId, StopCondition *_stopCondition) override {
        return new DefaultThreadLoop(_g, _rng, _threadId, _stopCondition, this->RQ_RANGE,
                                     argsGeneratorBuilder->build(_rng),
                                     parameters);
    }

    void toJson(nlohmann::json &json) const override {
        json["threadLoopType"] = ThreadLoopType::DEFAULT;
        json["insFrac"] = parameters->INS_FRAC;
        json["delFrac"] = parameters->DEL_FRAC;
        json["rqFrac"] = parameters->RQ;
        json["argsGeneratorBuilder"] = *argsGeneratorBuilder;
    }


    void fromJson(const nlohmann::json &j) override {
        parameters->INS_FRAC = j["insFrac"];
        parameters->DEL_FRAC = j["delFrac"];
        parameters->RQ = j["rqFrac"];
        argsGeneratorBuilder = getArgsGeneratorFromJson(j["argsGeneratorBuilder"]);
    }

    std::string toString(size_t indents = 1) override {
        return indented_title_with_str_data("Type", "Default", indents)
               + indented_title_with_data("INS_FRAC", parameters->INS_FRAC, indents)
               + indented_title_with_data("DEL_FRAC", parameters->DEL_FRAC, indents)
               + indented_title_with_data("RQ", parameters->RQ, indents)
               + indented_title("Args generator", indents)
               + argsGeneratorBuilder->toString(indents + 1);
    }

    ~DefaultThreadLoopBuilder() override = default;
};


#endif //SETBENCH_DEFAULT_THREAD_LOOP_H
