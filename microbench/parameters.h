//
// Created by Ravil Galiev on 22.02.2023.
//

#ifndef SETBENCH_PARAMETERS_OLD_H
#define SETBENCH_PARAMETERS_OLD_H

//#include "common.h"
#include "binding.h"
#include "thread_loops/thread_loop_builder.h"

struct Parameters_Old {
    bool isNonShuffle = false;

//    double INS_FRAC;
//    double DEL_FRAC;
//    double RQ;
    int RQSIZE;
    int MAXKEY = 0;
    int MILLIS_TO_RUN;
    int DESIRED_PREFILL_SIZE;
    bool PREFILL;
    int PREFILL_THREADS;
    int WORK_THREADS;
    int RQ_THREADS;
    int TOTAL_THREADS;

    PrefillType PREFILL_TYPE;
    int PREFILL_HYBRID_MIN_MS;
    int PREFILL_HYBRID_MAX_MS;

//    KeyGeneratorType keygenType;

    ThreadLoopBuilder *threadLoopBuilder;
//    ThreadLoopParameters *threadLoopParameters;

    Parameters_Old() {
        this->PREFILL_THREADS = 0;
        this->MILLIS_TO_RUN = 1000;
        this->RQ_THREADS = 0;
        this->WORK_THREADS = 4;
        this->RQSIZE = 0;
//        this->RQ = 0;
//        this->INS_FRAC = 0;
//        this->DEL_FRAC = 0;
        this->MAXKEY = 100000;
        this->PREFILL_HYBRID_MIN_MS = 1000;
        this->PREFILL_HYBRID_MAX_MS = 300000; // 5 minutes
        // note: DESIRED_PREFILL_SIZE is mostly useful for prefilling with in non-uniform distributions, to get sparse key spaces of a particular size
        this->DESIRED_PREFILL_SIZE = 0;  // note: -1 means "use whatever would be expected in the steady state"
        // to get NO prefilling, set -nprefill 0


        this->PREFILL_TYPE = PREFILL_MIXED;
    };

    virtual ~Parameters_Old() = default;

    virtual void build() {
        TOTAL_THREADS = WORK_THREADS + RQ_THREADS;
        threadLoopBuilder->build();
    }

    virtual std::string toString() {
        std::string params;

        params += "MILLIS_TO_RUN                 : " + std::to_string(MILLIS_TO_RUN) + "\n";
        params += threadLoopBuilder->toString(1);
//        params+="INS_FRAC                      : " + std::to_string(INS_FRAC) + "\n";
//        params+="DEL_FRAC                      : " + std::to_string(DEL_FRAC) + "\n";
//        params+="RQ                            : " + std::to_string(RQ) + "\n";
        params += "RQSIZE                        : " + std::to_string(RQSIZE) + "\n";
        params += "MAXKEY                        : " + std::to_string(MAXKEY) + "\n";
        params += "PREFILL_THREADS               : " + std::to_string(PREFILL_THREADS) + "\n";
        params += "DESIRED_PREFILL_SIZE          : " + std::to_string(DESIRED_PREFILL_SIZE) + "\n";
        params += "TOTAL_THREADS                 : " + std::to_string(TOTAL_THREADS) + "\n";
        params += "WORK_THREADS                  : " + std::to_string(WORK_THREADS) + "\n";
        params += "RQ_THREADS                    : " + std::to_string(RQ_THREADS) + "\n";

        if (isNonShuffle) {
            params += "DATA_TYPE                    : NON_SHUFFLE\n";
        }

        return params;
    }

    virtual void parseArg(ParseArgument *args) {
        if (strcmp(args->getCurrent(), "-rqsize") == 0) {
            this->RQSIZE = atoi(args->getNext());
            this->threadLoopBuilder->threadLoopParameters->RQ_RANGE = this->RQSIZE;
        } else if (strcmp(args->getCurrent(), "-k") == 0) {
            this->MAXKEY = atoi(args->getNext());
            this->threadLoopBuilder->threadLoopParameters->RQ_RANGE = this->MAXKEY;
            if (this->MAXKEY < 1) {
                setbench_error("key range cannot contain fewer than 1 key");
            }
        } else if (strcmp(args->getCurrent(), "-nrq") == 0) {
            this->RQ_THREADS = atoi(args->getNext());
        } else if (strcmp(args->getCurrent(), "-nwork") == 0) {
            this->WORK_THREADS = atoi(args->getNext());
        } else if (strcmp(args->getCurrent(), "-nprefill") == 0) { // num threads to prefill with
            this->PREFILL_THREADS = atoi(args->getNext());
        } else if (strcmp(args->getCurrent(), "-prefill-mixed") == 0) { // prefilling type
            this->PREFILL_TYPE = PrefillType::PREFILL_MIXED;
        } else if (strcmp(args->getCurrent(), "-prefill-insert") == 0) { // prefilling type
            this->PREFILL_TYPE = PrefillType::PREFILL_INSERT;
        } else if (strcmp(args->getCurrent(), "-prefill-hybrid") == 0) { // prefilling type
            this->PREFILL_TYPE = PrefillType::PREFILL_HYBRID;
        } else if (strcmp(args->getCurrent(), "-prefill-hybrid-min-ms") == 0) {
            this->PREFILL_HYBRID_MIN_MS = atoi(args->getNext());
        } else if (strcmp(args->getCurrent(), "-prefill-hybrid-max-ms") == 0) {
            this->PREFILL_HYBRID_MAX_MS = atoi(args->getNext());
        } else if (strcmp(args->getCurrent(), "-prefillsize") == 0) {
            this->DESIRED_PREFILL_SIZE = atol(args->getNext());
        } else if (strcmp(args->getCurrent(), "-t") == 0) {
            this->MILLIS_TO_RUN = atoi(args->getNext());
        } else if (strcmp(args->getCurrent(), "-non-shuffle") == 0) {
            this->isNonShuffle = true;
        } else if (strcmp(args->getCurrent(), "-pin") == 0) { // e.g., "-pin 1.2.3.8-11.4-7.0"
            binding_parseCustom(args->getNext()); // e.g., "1.2.3.8-11.4-7.0"
            std::cout << "parsed custom binding: " << args->getCurrent() << std::endl;
        } else if (!this->threadLoopBuilder->parseArg(args)) {
            std::cout << "bad argument: " << args->getCurrent() << "\nindex: " << args->pointer << std::endl;
            exit(1);
        }
    }

};

#endif //SETBENCH_PARAMETERS_OLD_H
