//
// Created by Ravil Galiev on 06.04.2023.
//

#ifndef SETBENCH_DEFAULT_THREAD_LOOP_PARAMETERS_H
#define SETBENCH_DEFAULT_THREAD_LOOP_PARAMETERS_H

#include "thread_loops/thread_loop_parameters.h"

struct DefaultThreadLoopParameters : public ThreadLoopParameters {
    double INS_FRAC;
    double DEL_FRAC;
    double RQ;

    DefaultThreadLoopParameters() {
        this->threadLoopType = ThreadLoopType::DEFAULT;
    }

    virtual std::string toString() {
        std::string params;
        params+="INS_FRAC                      : " + std::to_string(INS_FRAC) + "\n";
        params+="DEL_FRAC                      : " + std::to_string(DEL_FRAC) + "\n";
        params+="RQ                            : " + std::to_string(RQ) + "\n";

        return params;
    }

    virtual void build() {
    }

    virtual bool parseArg(ParseArgument * args) {
        if (strcmp(args->getCurrent(), "-i") == 0) {
            this->INS_FRAC = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-d") == 0) {
            this->DEL_FRAC = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-insdel") == 0) {
            this->INS_FRAC = atof(args->getNext());
            this->DEL_FRAC = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-rq") == 0) {
            this->RQ = atof(args->getNext());
        } else {
            return false;
        }
        return true;
    }
};

#endif //SETBENCH_DEFAULT_THREAD_LOOP_PARAMETERS_H
