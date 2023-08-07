//
// Created by Ravil Galiev on 21.07.2023.
//

#ifndef SETBENCH_BENCH_PARAMETERS_H
#define SETBENCH_BENCH_PARAMETERS_H

#include "globals_extern.h"
#include "parameters.h"
#include "workloads/stop_condition/impls/operation_counter.h"
#include "workloads/thread_loops/impls/default_thread_loop.h"

struct BenchParameters {
    size_t range;

    Parameters test;
    Parameters prefill;
    Parameters warmUp;

    BenchParameters() {
        range = 2048;
        test = Parameters();
//                ->addThreadLoopBuilder(*new DefaultThreadLoopBuilder(), 1);
        createDefaultPrefill();
        warmUp = Parameters();
    }

    BenchParameters &createDefaultPrefill() {
        prefill = Parameters();
        prefill.stopCondition = new OperationCounter(range / 2);
        prefill.addThreadLoopBuilder(
                *(new PrefillInsertThreadLoopBuilder()),
                1
        );
        return *this;
    }

    BenchParameters &setRange(size_t _range) {
        range = _range;
        return *this;
    }

    BenchParameters &setTest(Parameters &_test) {
        test = _test;
        return *this;
    }

    BenchParameters &setPrefill(Parameters &_prefill) {
        prefill = _prefill;
        return *this;
    }

    BenchParameters &setWarmUp(Parameters &_warmUp) {
        warmUp = _warmUp;
        return *this;
    }

    size_t getTotalThreads() {
        return prefill.getNumThreads() + warmUp.getNumThreads() + test.getNumThreads();
    }

    void init() {
        prefill.init(range);
        warmUp.init(range);
        test.init(range);
    }

    std::string toString(size_t indents = 1) {
        return indented_title_with_data("Range", range, indents)
               + (prefill.getNumThreads() == 0
                  ? toStringStage("without prefill")
                  : toStringStage("prefill parameters") +
                    prefill.toString(indents + 1))
               + (warmUp.getNumThreads() == 0
                  ? toStringStage("without warmUp")
                  : toStringStage("warmUp parameters") +
                    warmUp.toString(indents + 1))
               + toStringStage("test parameters")
               + test.toString(indents + 1);
    }

//    nlohmann::json toJson() {
//        nlohmann::json json;
//        json["range"] = range;
//        json["test"] = test->toJson();
//        json["prefill"] = prefill->toJson();
//        json["warmUp"] = warmUp->toJson();
//        return json;
//    }
};

void to_json(nlohmann::json &json, const BenchParameters &s) {
    json["range"] = s.range;
    json["test"] = s.test;
    json["prefill"] = s.prefill;
    json["warmUp"] = s.warmUp;
}

void from_json(const nlohmann::json &json, BenchParameters &s) {
    s.range = json["range"];
//    Parameters p = ;
    s.test = json["test"];//json["test"];
    s.prefill = json["prefill"];//json["test"];
    s.warmUp = json["warmUp"];//json["test"];
//    json["prefill"] = s.prefill->toJson();
//    json["warmUp"] = s.warmUp->toJson();

}

#endif //SETBENCH_BENCH_PARAMETERS_H
