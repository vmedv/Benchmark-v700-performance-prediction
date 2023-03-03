//
// Created by Ravil Galiev on 22.02.2023.
//

#ifndef SETBENCH_PARAMETERS_H
#define SETBENCH_PARAMETERS_H

#include "common.h"
#include "binding.h"

struct Parameters {
    bool isNonShuffle = false;

    double INS_FRAC;
    double DEL_FRAC;
    double RQ;
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

    Parameters() = default;

    virtual ~Parameters() = default;

    virtual std::string toString() {
        std::string params;

        params+="MILLIS_TO_RUN                 : " + std::to_string(MILLIS_TO_RUN) + "\n";
        params+="INS_FRAC                      : " + std::to_string(INS_FRAC) + "\n";
        params+="DEL_FRAC                      : " + std::to_string(DEL_FRAC) + "\n";
        params+="RQ                            : " + std::to_string(RQ) + "\n";
        params+="RQSIZE                        : " + std::to_string(RQSIZE) + "\n";
        params+="MAXKEY                        : " + std::to_string(MAXKEY) + "\n";
        params+="PREFILL_THREADS               : " + std::to_string(PREFILL_THREADS) + "\n";
        params+="DESIRED_PREFILL_SIZE          : " + std::to_string(DESIRED_PREFILL_SIZE) + "\n";
        params+="TOTAL_THREADS                 : " + std::to_string(TOTAL_THREADS) + "\n";
        params+="WORK_THREADS                  : " + std::to_string(WORK_THREADS) + "\n";
        params+="RQ_THREADS                    : " + std::to_string(RQ_THREADS) + "\n";

        if (isNonShuffle) {
            params+="DATA_TYPE                    : NON_SHUFFLE\n";
        }

        return params;
    }
};

#endif //SETBENCH_PARAMETERS_H
