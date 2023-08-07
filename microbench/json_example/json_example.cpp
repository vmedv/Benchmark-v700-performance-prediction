//
// Created by Ravil Galiev on 25.07.2023.
//

#ifndef SETBENCH_JSON_EXAMPLE_CPP
#define SETBENCH_JSON_EXAMPLE_CPP

#include <fstream>
#include <iostream>
#include "workloads/args_generators/impls/default_args_generator.h"
#include "workloads/thread_loops/impls/default_thread_loop.h"
#include "workloads/bench_parameters.h"

typedef long long ll;

//g++ -I. -I../common -I.. json_test2.cpp -o b.out

int main() {
    /**
     * The first step is the creation the BenchParameters class.
     */

    BenchParameters benchParameters;

    /**
     * Set the range of keys.
     */

    benchParameters.setRange(2048);

    /**
     * Create the Parameters class for benchmarking (test).
     */

    Parameters test;

    /**
     * We will need to set the stop condition and workloads.
     *
     * First, let's create a stop condition: Timer with 10 second (10000 millis).
     */

    StopCondition *stopCondition = new Timer(10000);

    /**
     * Setup a workload.
     *
     *
     */

    ArgsGeneratorBuilder *argsGeneratorBuilder
            = (new DefaultArgsGeneratorBuilder())
                    ->setDistributionBuilder(
                            (new ZipfDistributionBuilder())
                                    ->setAlpha(1.0)
                    )
                    ->setDataMapBuilder(
                            new ArrayDataMapBuilder()
                    );

    ThreadLoopBuilder *threadLoopBuilder
            = (new DefaultThreadLoopBuilder())
                    ->setInsFrac(0.1)
                    ->setRemFrac(0.1)
                    ->setRq(0)
                    ->setArgsGeneratorBuilder(argsGeneratorBuilder);

    test.addThreadLoopBuilder(*threadLoopBuilder, 8)
            .setStopCondition(stopCondition);

    benchParameters.setTest(test)
            .createDefaultPrefill();

    std::cout << "to json\n";

    nlohmann::json json = benchParameters;

    std::ofstream out("json_example.txt");

    out << json.dump(4);

    std::cout << "end\n";

}

#endif //SETBENCH_JSON_EXAMPLE_CPP
