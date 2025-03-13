#include <cstdint>
#include <iostream>
#include <fstream>
#include "globals_extern.h"
#include "random_xoshiro256p.h"
#include "globals_t.h"
#include "globals_t_impl.h"
#include "papi_util.h"
#include "server_clock.h"
#include "statistics.h"
#include "parse_argument.h"
#include "workloads/bench_parameters.h"
#include "debugprinting.h"
#include "adapter.h"
#include "plaf.h"

template <typename T>
T* ParseJsonFile(const std::string& file_name) {
    std::ifstream fin;
    fin.open(file_name);

    nlohmann::json j = nlohmann::json::parse(fin);
		std::cout << j.dump(4) << std::endl;
    T* t = new T(j);

    fin.close();
    return t;
}

void BindThreads(int nthreads) {
    // setup thread pinning/binding

    binding_configurePolicy(nthreads);
    std::cout << "ACTUAL_THREAD_BINDINGS=";
    for (int i = 0; i < nthreads; ++i) {
        std::cout << (i ? "," : "") << binding_getActualBinding(i);
    }
    std::cout << std::endl;
    //    if (!binding_isInjectiveMapping(nthreads)) {
    //        std::cout << "ERROR: thread binding maps more than one thread to a single logical
    //        processor" << std::endl; exit(-1);
    //    }
}

void CreateDataStructure(globals_t* g) {
    g->dsAdapter = new DS_ADAPTER_T(g->benchParameters->getMaxThreads(), g->KEY_MIN, g->KEY_MAX,
                                    g->NO_VALUE, g->rngs);
}

void Execute(globals_t* g, Parameters* parameters) {
    std::thread** threads = new std::thread*[MAX_THREADS_POW2];
    ThreadLoop** thread_loops = parameters->getWorkload(g, g->rngs);

    std::cout << "binding threads...\n";
    binding_setCustom(parameters->getPin());
    BindThreads(parameters->getNumThreads());

    std::cout << "creating threads...\n";
		nlohmann::json json;
		std::mutex m;

    for (int i = 0; i < parameters->getNumThreads(); ++i) {
        threads[i] = new std::thread(&ThreadLoop::run, thread_loops[i]);
    }

    while (g->running < parameters->getNumThreads()) {
        TRACE COUTATOMIC("main thread: waiting for threads to START running=" << g->running
                                                                              << std::endl);
    }  // wait for all threads to be ready

    ////////////////////////////////////

    SOFTWARE_BARRIER;
    g->startTime = std::chrono::high_resolution_clock::now();
    g->startClockTicks = get_server_clock();
    SOFTWARE_BARRIER;
#ifdef MEASURE_TIMELINE_STATS
    ___timeline_use = 1;
#endif

    parameters->stopCondition->start(parameters->getNumThreads());
    g->start = true;
    SOFTWARE_BARRIER;

    for (size_t i = 0; i < parameters->getNumThreads(); ++i) {
        threads[i]->join();
    }

    SOFTWARE_BARRIER;
    g->done = true;
    __sync_synchronize();
    g->endTime = std::chrono::high_resolution_clock::now();
    __sync_synchronize();
    printUptimeStampForPERF("END")

    // TODO TIME_IS_UP
#if defined TIME_IS_UP
        DEBUG_PRINT_ARENA_STATS;
    COUTATOMIC(std::endl);
    COUTATOMIC(toStringBigStage("TIME IS UP"))
    COUTATOMIC(std::endl);

    const long MAX_NAPPING_MILLIS = (parameters->MAXKEY > 5e7 ? 120000 : 30000);
    g->elapsedMillis = duration_cast<milliseconds>(g->endTime - g->startTime).count();
    g->elapsedMillisNapping = 0;
    while (g->running > 0 && g->elapsedMillisNapping < MAX_NAPPING_MILLIS) {
        nanosleep(&tsNap, NULL);
        g->elapsedMillisNapping =
            duration_cast<milliseconds>(high_resolution_clock::now() - g->startTime).count() -
            g->elapsedMillis;
    }

    if (g->running > 0) {
        COUTATOMIC(std::endl);
        COUTATOMIC("Validation FAILURE: "
                   << g->running
                   << " non-terminating thread(s) [did we exhaust physical memory and experience "
                      "excessive slowdown due to swap mem?]"
                   << std::endl);
        COUTATOMIC(std::endl);
        COUTATOMIC("elapsedMillis=" << g->elapsedMillis << " elapsedMillisNapping="
                                    << g->elapsedMillisNapping << std::endl);

        if (g->dsAdapter->validateStructure()) {
            std::cout << "Structural validation OK" << std::endl;
        } else {
            std::cout << "Structural validation FAILURE." << std::endl;
        }

#if defined USE_GSTATS && defined OVERRIDE_PRINT_STATS_ON_ERROR
        GSTATS_PRINT;
        std::cout << std::endl;
#endif

        g->dsAdapter->printSummary();
#ifdef RQ_DEBUGGING_H
        DEBUG_VALIDATE_RQ(TOTAL_THREADS);
#endif
        exit(-1);
    }
#endif

    g->elapsedMillis =
        std::chrono::duration_cast<std::chrono::milliseconds>(g->endTime - g->startTime).count();

    parameters->stopCondition->clean();
    delete[] threads;
    delete[] thread_loops;
    binding_deinit();

    g->start = false;
    g->done = false;
}

int main(int argc, char** argv) {
    std::cout << "binary=" << argv[0] << std::endl;

    BenchParameters* bench_parameters = new BenchParameters();
    Parameters* prefill = nullptr;
    int64_t range = -1;

    ParseArgument args = ParseArgument(argc, argv).next();
    bool create_default_prefill = false;
    while (args.hasNext()) {
        if (strcmp(args.getCurrent(), "-json-file") == 0) {
            delete bench_parameters;
            bench_parameters = ParseJsonFile<BenchParameters>(args.getNext());
        } else if (strcmp(args.getCurrent(), "-prefill") == 0) {
            prefill = ParseJsonFile<Parameters>(args.getNext());
        } else if (strcmp(args.getCurrent(), "-range") == 0) {
            range = atoll(args.getNext());
        } else if (strcmp(args.getCurrent(), "-create-default-prefill") == 0) {
            create_default_prefill = true;
        } else {
            std::cerr << "Unexpected option: " << args.getCurrent() << "\nindex: " << args.pointer
                      << ". Ignoring..." << std::endl;
        }
        args.next();
    }
		std::cout << "args parsed" << std::endl;

    if (prefill != nullptr) {
        bench_parameters->setPrefill(prefill);
    }
    if (range != -1) {
        bench_parameters->setRange(range);
    }
    if (create_default_prefill) {
        if (prefill == nullptr) {
            bench_parameters->createDefaultPrefill();
        } else {
            std::cerr << "WARNING: The \'-prefill\' argument was already specified. Ignoring...\n";
        }
    }

    PRINTS(DS_TYPENAME)
    PRINTS(FIND_FUNC)
    PRINTS(INSERT_FUNC)
    PRINTS(ERASE_FUNC)
    PRINTS(GET_FUNC)
    PRINTS(RQ_FUNC)
    PRINTS(RECLAIM)
    PRINTS(ALLOC)
    PRINTS(POOL)
    PRINTS(MAX_THREADS_POW2)
    PRINTS(CPU_FREQ_GHZ)

    std::cout << "\ninitialization of parameters...\n";

    bench_parameters->init();

    std::cout << std::endl;

    COUTATOMIC(toStringBigStage("BENCH PARAMETERS"))

    std::cout << std::endl;

    std::cout << bench_parameters->toString();

    std::cout << std::endl;

    globals_t* g = new globals_t(bench_parameters);

    g->programExecutionStartTime = std::chrono::high_resolution_clock::now();

    // print object sizes, to help debugging/sanity checking memory layouts
    g->dsAdapter->printObjectSizes();

    int total_threads = g->benchParameters->getTotalThreads();

    using namespace std::chrono;
    papi_init_program(total_threads);
    // create the actual data structure

    CreateDataStructure(g);

    COUTATOMIC(std::endl);
    COUTATOMIC(toStringBigStage("BEGIN RUNNING"))
    COUTATOMIC(std::endl);

    /**
     * PREFILL STAGE
     */
    if (g->benchParameters->prefill->getNumThreads() != 0) {
        COUTATOMIC(toStringStage("Prefill stage"))

        Execute(g, g->benchParameters->prefill);

        {
            // print prefilling status information
            using namespace std::chrono;
            const int64_t total_updates = GSTATS_OBJECT_NAME.get_sum<int64_t>(num_inserts) +
                                           GSTATS_OBJECT_NAME.get_sum<int64_t>(num_removes);
            g->curKeySum += GSTATS_OBJECT_NAME.get_sum<int64_t>(key_checksum);
            g->curSize += GSTATS_OBJECT_NAME.get_sum<int64_t>(num_successful_inserts) -
                          GSTATS_OBJECT_NAME.get_sum<int64_t>(num_successful_removes);
            auto elapsed_millis = duration_cast<milliseconds>(g->endTime - g->startTime).count();
            COUTATOMIC("finished prefilling to size "
                       << g->curSize  // << " for expected size "// << expectedSize
                       << " keysum=" << g->curKeySum << ", performing " << total_updates
                       << " updates; total_prefilling_elapsed_ms=" << elapsed_millis << " ms)"
                       << std::endl)
            std::cout << "prefill_millis=" << elapsed_millis << std::endl;
            GSTATS_CLEAR_ALL;

            // print total prefilling time
            g->dsAdapter->printSummary();  ///////// debug
        }

    } else {
        COUTATOMIC(toStringStage("Without Prefill stage"))
    }

}
