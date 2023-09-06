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

struct ThreadLoopSettings {
    ThreadLoopBuilder *threadLoopBuilder;
    size_t quantity;
    int *pin;

    ThreadLoopSettings(const nlohmann::json &j) {
        quantity = j["quantity"];
        pin = new int[quantity];
        if (j["pin"] != nullptr) {
            std::copy(std::begin(j["pin"]), std::end(j["pin"]), pin);
        } else {
            pin = nullptr;
        }
        threadLoopBuilder = getThreadLoopFromJson(j["threadLoopBuilder"]);
    }

    ThreadLoopSettings *setThreadLoopBuilder(ThreadLoopBuilder *_threadLoopBuilder) {
        threadLoopBuilder = _threadLoopBuilder;
        return this;
    }

    ThreadLoopSettings *setQuantity(size_t _quantity) {
        quantity = _quantity;
        return this;
    }

    ThreadLoopSettings *setPin(int *_pin) {
        pin = _pin;
        return this;
    }

    ThreadLoopSettings() {}

    ThreadLoopSettings(ThreadLoopBuilder *threadLoopBuilder, size_t quantity = 1, int *pin = nullptr)
            : threadLoopBuilder(threadLoopBuilder), quantity(quantity), pin(pin) {}

    ~ThreadLoopSettings() {
        delete threadLoopBuilder;
        delete pin;
    }
};


void to_json(nlohmann::json &j, const ThreadLoopSettings &s) {
    j["quantity"] = s.quantity;
    j["threadLoopBuilder"] = *s.threadLoopBuilder;
    if (s.pin == nullptr) {
        j["pin"] = nullptr;
    } else {
        for (size_t i = 0; i < s.quantity; ++i) {
            j["pin"].push_back(s.pin[i]);
        }
    }
}

void from_json(const nlohmann::json &j, ThreadLoopSettings &s) {
    s.quantity = j["quantity"];
    s.pin = new int[s.quantity];
    if (j["pin"] != nullptr) {
        std::copy(std::begin(j["pin"]), std::end(j["pin"]), s.pin);
    } else {
        std::fill(s.pin, s.pin + s.quantity, -1);
    }
    s.threadLoopBuilder = getThreadLoopFromJson(j["threadLoopBuilder"]);
}

class Parameters {
    size_t numThreads;
    std::vector<int> pin;
public:
    StopCondition *stopCondition;

    std::vector<ThreadLoopSettings *> threadLoopBuilders;

    Parameters() : numThreads(0), stopCondition(new Timer(5000)) {}

    Parameters(const Parameters &p) = default;

    size_t getNumThreads() const {
        return numThreads;
    }

    const std::vector<int> &getPin() const {
        return pin;
    }

    Parameters *setStopCondition(StopCondition *_stopCondition) {
        stopCondition = _stopCondition;
        return this;
    }

    Parameters *setThreadLoopBuilders(const std::vector<ThreadLoopSettings *> &_threadLoopBuilders) {
        Parameters::threadLoopBuilders = _threadLoopBuilders;
        return this;
    }

    Parameters *addThreadLoopBuilder(ThreadLoopSettings *_threadLoopSettings) {
        threadLoopBuilders.push_back(_threadLoopSettings);
        numThreads += _threadLoopSettings->quantity;
        if (_threadLoopSettings->pin != nullptr) {
            for (size_t i = 0; i < _threadLoopSettings->quantity; ++i) {
                pin.push_back(_threadLoopSettings->pin[i]);
            }
        } else {
            for (size_t i = 0; i < _threadLoopSettings->quantity; ++i) {
                pin.push_back(-1);
            }
        }
        return this;
    }

    Parameters *addThreadLoopBuilder(ThreadLoopBuilder *_threadLoopBuilder,
                                     size_t quantity = 1,
                                     int *_pin = nullptr) {
        return addThreadLoopBuilder(new ThreadLoopSettings(_threadLoopBuilder, quantity, _pin));
    }

    Parameters *init(int range) {
        for (ThreadLoopSettings *threadLoopSettings: threadLoopBuilders) {
            threadLoopSettings->threadLoopBuilder->init(range);
        }
        return this;
    }

    ThreadLoop **getWorkload(globals_t *_g, Random64 *_rngs) const {
        ThreadLoop **workload = new ThreadLoop *[this->numThreads];
        for (size_t threadId = 0, i = 0, curQuantity = 0; threadId < this->numThreads; ++threadId, ++curQuantity) {
            if (curQuantity >= threadLoopBuilders[i]->quantity) {
                curQuantity = 0;
                ++i;
            }

            workload[threadId] = threadLoopBuilders[i]->threadLoopBuilder
                    ->build(_g, _rngs[threadId], threadId, stopCondition);
        }
        return workload;
    }

    void toJson(nlohmann::json &j) const {
        j["numThreads"] = numThreads;
        j["pin"] = pin;
        j["stopCondition"] = *stopCondition;
        for (ThreadLoopSettings *tls: threadLoopBuilders) {
            j["threadLoopBuilders"].push_back(*tls);
        }
    }

    void fromJson(const nlohmann::json &j) {
        stopCondition = getStopConditionFromJson(j["stopCondition"]);


        for (const auto &i: j["threadLoopBuilders"]) {
            addThreadLoopBuilder(new ThreadLoopSettings(i));
        }
    }

    std::string toString(size_t indents = 1) {
        std::string result = indented_title_with_data("number of threads", numThreads, indents)
                             + indented_title("stop condition", indents)
                             + stopCondition->toString(indents + 1)
                             + indented_title_with_data("number of workloads", threadLoopBuilders.size(), indents);


        std::string pin_string = std::to_string(pin[0]);
        for (size_t i = 1; i < numThreads; ++i) {
            pin_string += "," + std::to_string(pin[i]);
        }

        result += indented_title_with_str_data("all pins", pin_string, indents)
                  + indented_title("thread loops", indents);


        for (auto tls: threadLoopBuilders) {
            result += indented_title_with_data("quantity", tls->quantity, indents + 1);

            if (tls->pin != nullptr) {
                pin_string = std::to_string(tls->pin[0]);
                for (size_t i = 1; i < tls->quantity; ++i) {
                    pin_string += "," + std::to_string(tls->pin[i]);
                }

                result += indented_title_with_str_data("pin", pin_string, indents + 1);
            }

            result += tls->threadLoopBuilder->toString(indents + 2);
        }
        return result;
    }

    ~Parameters() {
        delete stopCondition;
        for (ThreadLoopSettings *tls : threadLoopBuilders) {
            delete tls;
        }
    }

};

void to_json(nlohmann::json &json, const Parameters &s) {
    s.toJson(json);
}

void from_json(const nlohmann::json &j, Parameters &s) {
    s.fromJson(j);
}

#endif //SETBENCH_PARAMETERS_H
