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
//
//    ArgsGeneratorBuilder *argsGeneratorBuilder
//        = (new TemporarySkewedArgsGeneratorBuilder())
//                ->setSetNumber(5)
//                    ->setHotTimes(new long long[5]{1, 2, 3, 4, 5})
//                    ->setRelaxTimes(new long long[5]{1, 2, 3, 4, 5})
//                    ->setHotSizeAndRatio(0, 0.1, 0.8)
//                    ->setHotSizeAndRatio(1, 0.2, 0.7)
//                    ->setHotSizeAndRatio(2, 0.3, 0.6)
//                    ->setHotSizeAndRatio(3, 0.4, 0.6)
//                    ->setHotSizeAndRatio(4, 0.5, 0.7)
//                    ->enableManualSettingSetBegins()
//                    ->setSetBegins(new double [5]{0, 0.1, 0.2, 0.3, 0.05});

//    ArgsGeneratorBuilder *argsGeneratorBuilder
//            = (new CreakersAndWaveArgsGeneratorBuilder())
//                ->setCreakersRatio(0.2)
//                ->setWaveSize(0.2)
//                ->setCreakersSize(0.1)
//                ->setDataMapBuilder(new IdDataMapBuilder());



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
