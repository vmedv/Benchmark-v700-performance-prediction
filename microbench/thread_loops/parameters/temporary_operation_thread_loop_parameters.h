//
// Created by Ravil Galiev on 06.04.2023.
//

#ifndef SETBENCH_TEMPORARY_OPERATION_THREAD_LOOP_PARAMETERS_H
#define SETBENCH_TEMPORARY_OPERATION_THREAD_LOOP_PARAMETERS_H

#include "thread_loops/thread_loop_parameters.h"

struct TemporaryOperationThreadLoopParameters : public ThreadLoopParameters {
    size_t tempOperCount = 0;
    size_t *opTimes;
    double *INS_FRACs;
    double *DEL_FRACs;
    double *RQs;

    TemporaryOperationThreadLoopParameters() {
        this->threadLoopType = ThreadLoopType::TEMPORARY_OPERATION;
    }

    virtual std::string toString() {
        std::string params;
        params += "THREAD_LOOP                   : TEMPORARY_OPERATION\n";
        params += "NUMBER_OF_TEMPS               : " + std::to_string(tempOperCount) + "\n";

        params += "WRITE_RATIOS:                    \n";
        for (int i = 0; i < tempOperCount; i++) {
            params += "    TIME_OF_" + std::to_string(i) + "                 : " +
                      std::to_string(opTimes[i]) + "\n" +
                      "    INS_FRAC_OF_" + std::to_string(i) + "             : " +
                    std::to_string(INS_FRACs[i]) + "\n" +
                      "    DEL_FRAC_OF_" + std::to_string(i) + "             : " +
                    std::to_string(DEL_FRACs[i]) + "\n" +
                      "    RQ_OF_" + std::to_string(i) + "                   : " +
                    std::to_string(RQs[i]) + "\n";
        }

        return params;
    }

    void setTempOperCount(size_t _tempOperCount) {
        tempOperCount = _tempOperCount;
        opTimes = new size_t[tempOperCount];
        INS_FRACs = new double[tempOperCount];
        DEL_FRACs = new double[tempOperCount];
        RQs = new double[tempOperCount];
    }

    void setOpTime(const size_t i, const size_t _opTime) {
        assert(i < tempOperCount);
        opTimes[i] = _opTime;
    }

    void setInsFrac(const size_t i, const double _insFrac) {
        assert(i < tempOperCount);
        INS_FRACs[i] = _insFrac;
    }

    void setDelFrac(const size_t i, const double _delFrac) {
        assert(i < tempOperCount);
        DEL_FRACs[i] = _delFrac;
    }

    void setRQ(const size_t i, const double _RQ) {
        assert(i < tempOperCount);
        RQs[i] = _RQ;
    }

    virtual void build() {
    }

    virtual bool parseArg(ParseArgument *args) {
        if (strcmp(args->getCurrent(), "-temp-oper-count") == 0) {
            this->setTempOperCount(atoi(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-i") == 0) {
            int pointer = atoi(args->getNext());
            this->setInsFrac(pointer, atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-d") == 0) {
            int pointer = atoi(args->getNext());
            this->setDelFrac(pointer, atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-insdel") == 0) {
            int pointer = atoi(args->getNext());
            this->setInsFrac(pointer, atof(args->getNext()));
            this->setDelFrac(pointer, atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-rq") == 0) {
            int pointer = atoi(args->getNext());
            this->setRQ(pointer, atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-ot") == 0) {
            int pointer = atoi(args->getNext());
            this->setOpTime(pointer, atoi(args->getNext()));
        } else {
            return false;
        }
        return true;
    }

};

#endif //SETBENCH_TEMPORARY_OPERATION_THREAD_LOOP_PARAMETERS_H
