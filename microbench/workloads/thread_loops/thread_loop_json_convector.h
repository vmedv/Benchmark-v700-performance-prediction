//
// Created by Ravil Galiev on 27.07.2023.
//

#ifndef SETBENCH_THREAD_LOOP_JSON_CONVECTOR_H
#define SETBENCH_THREAD_LOOP_JSON_CONVECTOR_H

#include "thread_loop_builder.h"
#include "workloads/thread_loops/impls/default_thread_loop.h"
#include "workloads/thread_loops/impls/prefill_insert_thread_loop.h"


ThreadLoopBuilder *getThreadLoopFromJson(const nlohmann::json &j) {
    ThreadLoopType type = j["threadLoopType"];
    ThreadLoopBuilder * threadLoopBuilder;
    switch (type) {
        case ThreadLoopType::DEFAULT:
            threadLoopBuilder = new DefaultThreadLoopBuilder();
            break;
        case ThreadLoopType::TEMPORARY_OPERATION:
            break;
        case ThreadLoopType::PREFILL_INSERT:
            threadLoopBuilder = new PrefillInsertThreadLoopBuilder();
            break;
    }
    threadLoopBuilder->fromJson(j);
    return threadLoopBuilder;
}

#endif //SETBENCH_THREAD_LOOP_JSON_CONVECTOR_H
