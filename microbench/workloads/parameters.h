//
// Created by Ravil Galiev on 21.07.2023.
//

#ifndef SETBENCH_PARAMETERS_H
#define SETBENCH_PARAMETERS_H

#include <vector>
#include "workloads/stop_condition/stop_condition.h"
#include "workloads/stop_condition/impls/timer.h"
#include "workloads/thread_loops/thread_loop_builder.h"
#include "workloads/thread_loops/impls/default_thread_loop.h"
#include "globals_t.h"
#include "workloads/thread_loops/thread_loop.h"
#include "workloads/stop_condition/stop_condition_json_convector.h"
#include "workloads/thread_loops/thread_loop_json_convector.h"
#include "binding.h"


void to_json(nlohmann::json &j, const std::pair<ThreadLoopBuilder &, size_t> &s) {
    j["quantity"] = s.second;
    j["threadLoopBuilder"] = s.first;
}


void from_json(const nlohmann::json &j, std::pair<ThreadLoopBuilder &, size_t> &s) {
    s.second = j["quantity"];
    s.first = *getThreadLoopFromJson(j["threadLoopBuilder"]);
}

class Parameters {
    size_t numThreads;
public:
    std::string pin; // e.g., "1.2.3.8-11.4-7.0"
    StopCondition *stopCondition;

    std::vector<std::pair<ThreadLoopBuilder &, size_t>> threadLoopBuilders;
//    std::map

    Parameters() : numThreads(0), stopCondition(new Timer(5000)) {
//        threadLoopBuilders.push_back(new DefaultThreadLoopBuilder<K>());
    }

//    Parameters *setNumThreads(int _numThreads) {
//        numThreads = _numThreads;
//        return this;
//    }

    size_t getNumThreads() const {
        return numThreads;
    }

    Parameters *setStopCondition(StopCondition *_stopCondition) {
        stopCondition = _stopCondition;
        return this;
    }

    Parameters *setThreadLoopBuilders(
            const std::vector<std::pair<ThreadLoopBuilder &, size_t>> &_threadLoopBuilders) {
        Parameters::threadLoopBuilders = _threadLoopBuilders;
        return this;
    }

    Parameters *addThreadLoopBuilder(ThreadLoopBuilder &_threadLoopBuilder, size_t quantity = 1) {
        threadLoopBuilders.push_back({_threadLoopBuilder, quantity});
        numThreads += quantity;
        return this;
    }

    void init(int range) {
        if (!pin.empty()) {
            binding_parseCustom(pin); // e.g., "1.2.3.8-11.4-7.0"
            std::cout << "parsed custom binding: " << pin << std::endl;
        }
        for (std::pair<ThreadLoopBuilder &, size_t> threadLoopBuilder: threadLoopBuilders) {
            threadLoopBuilder.first.init(range);
        }
    }

//    template<class GlobalsT>
    ThreadLoop **getWorkload(globals_t *_g, Random64 *_rngs) const {
        ThreadLoop **workload = new ThreadLoop *[this->numThreads];
        for (size_t threadId = 0, i = 0, curQuantity = 0; threadId < this->numThreads; ++threadId, ++curQuantity) {
            if (curQuantity >= threadLoopBuilders[i].second) {
                curQuantity = 0;
                ++i;
            }

            workload[threadId] = threadLoopBuilders[i].first
                    .build(_g, _rngs[threadId], threadId, stopCondition);
        }
        return workload;
    }


    void toJson(nlohmann::json &j) const {
        j["numThreads"] = numThreads;
        j["pin"] = pin;
        j["stopCondition"] = *stopCondition;
//        ThreadLoopBuilder * threadLoopBuilder = threadLoopBuilders[0].first;
//        for (std::pair<ThreadLoopBuilder&, size_t> i : threadLoopBuilders){
//
//        }
        j["threadLoopBuilders"] = threadLoopBuilders;
    }

    void fromJson(const nlohmann::json &j) {
//        numThreads = j["numThreads"];
        pin = j["pin"];
        stopCondition = getStopConditionFromJson(j["stopCondition"]);
//        threadLoopBuilders = j["threadLoopBuilders"];
//        threadLoopBuilders = j["threadLoopBuilders"].get<std::vector<std::pair<ThreadLoopBuilder&, size_t>> >();

//        ThreadLoopBuilder * threadLoopBuilder = threadLoopBuilders[0].first;
        for (const auto &i: j["threadLoopBuilders"]) {
            ThreadLoopBuilder &threadLoopBuilder = *getThreadLoopFromJson(i["threadLoopBuilder"]);
//            std::pair<ThreadLoopBuilder&, size_t> p = i.get<std::pair<ThreadLoopBuilder&, size_t>>();
            addThreadLoopBuilder(threadLoopBuilder, i["quantity"]);
//            threadLoopBuilders.push_back(i);
        }
//        j["threadLoopBuilders"] = threadLoopBuilders;

    }

    std::string toString(size_t indents = 1) {
        std::string result = indented_title_with_data("number of threads", numThreads, indents)
                             + indented_title("stop condition", indents)
                             + stopCondition->toString(indents + 1)
                             + indented_title_with_data("number of workloads", threadLoopBuilders.size(), indents)
                             + indented_title("thread loops", indents);

        for (auto tlb: threadLoopBuilders) {
            result += indented_title_with_data("quantity", tlb.second, indents + 1)
                      + tlb.first.toString(indents + 1);
        }
        return result;
    }

//    nlohmann::json toJson() {
//        nlohmann::json json;
//        json["numThreads"] = numThreads;
//        json["stopCondition"] = *stopCondition;
//        ThreadLoopBuilder * threadLoopBuilder = threadLoopBuilders[0].first;
//        json["threadLoopBuilders"] = threadLoopBuilders;
//        return json;
//    }

//    static Parameters * fromJson(const nlohmann::json &json) {
//        Parameters * p = new Parameters();
//        p->numThreads = json["numThreads"];
//        json["stopCondition"] = stopCondition->toJson();
//        ThreadLoopBuilder * threadLoopBuilder = threadLoopBuilders[0].first;
//        std::vector<std::pair<DefaultThreadLoopBuilder&, size_t>> a = json["threadLoopBuilders"];
//    }
};

void to_json(nlohmann::json &json, const Parameters &s) {
    s.toJson(json);
//    json["numThreads"] = s.numThreads;
//    json["stopCondition"] = s.stopCondition->toJson();
//    ThreadLoopBuilder * threadLoopBuilder = s.threadLoopBuilders[0].first;
//    json["threadLoopBuilders"] = *threadLoopBuilder;
}

void from_json(const nlohmann::json &j, Parameters &s) {
    s.fromJson(j);
}

#endif //SETBENCH_PARAMETERS_H
