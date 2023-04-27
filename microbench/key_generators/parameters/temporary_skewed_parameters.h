//
// Created by Ravil Galiev on 25.08.2022.
//

#ifndef SETBENCH_TEMPORARY_SKEWED_PARAMETERS_H
#define SETBENCH_TEMPORARY_SKEWED_PARAMETERS_H

#include "common.h"

/**
    n — { xi — yi — ti — rti } // либо   n — rt — { xi — yi — ti }
        n — количество элементов
        xi — процент элементов i-ого множества
        yi — вероятность чтения элемента из i-ого множества
          // 100% - yi — чтение остальных элементов
        ti — время / количество итераций работы в режиме горячего вызова i-ого множества
        rti / rt (relax time) — время / количество итераций работы в обычном режиме (равномерное распределение на все элементы)
          // rt — если relax time всегда одинаковый
          // rti — relax time после горячей работы с i-ым множеством
 */

struct TemporarySkewedParameters : public Parameters {
    size_t setCount = 0;
    double *setSizes;
    double *hotProbs;
    int *hotTimes;
    int *relaxTimes;
    int hotTime = -1;
    int relaxTime = -1;

    double *setBegins;

    virtual std::string toString() {
        std::string params = Parameters::toString();


        params += "KEY_GENERATOR                 : TEMPORARY_SKEWED\n";
        params += "NUMBER_OF_SETS                : " + std::to_string(setCount) + "\n";

        params += "SETS_SIZES:\n";
        for (int i = 0; i < setCount; i++) {
            params += "    SET_SIZE_" + std::to_string(i) + "                : "
                      + std::to_string(setSizes[i]) + "\n";
        }

        params += "COMMON_HOT_TIME               : " + std::to_string(hotTime) + "\n";
        params += "HOT_TIMES:\n";
        for (int i = 0; i < setCount; i++) {
            params += "    HOT_TIMES_" + std::to_string(i) + "               : "
                      + std::to_string(hotTimes[i]) + "\n";
        }

        params += "HOT_PROBABILITIES:\n";
        for (int i = 0; i < setCount; i++) {
            params += "    HOT_PROBABILITY_" + std::to_string(i) + "         : "
                      + std::to_string(hotProbs[i]) + "\n";
        }

        params += "COMMON_RELAX_TIME             : " + std::to_string(relaxTime) + "\n";
        params += "RELAX_TIMES:\n";
        for (int i = 0; i < setCount; i++) {
            params += "    RELAX_TIME_" + std::to_string(i) + "              : "
                      + std::to_string(relaxTimes[i]) + "\n";
        }

        if (isNonShuffle) {
            params += "SETS_BEGINS:\n";
            for (int i = 0; i < setCount; i++) {
                params += "    SET_BEGIN_" + std::to_string(i) + "               : "
                          + std::to_string(setBegins[i]) + "\n";
            }
        }

        return params;
    }

    void setSetCount(const size_t _setCount) {
        setCount = _setCount;
        setSizes = new double[setCount];
        hotProbs = new double[setCount];
        hotTimes = new int[setCount];
        relaxTimes = new int[setCount];

        if (isNonShuffle) {
            setBegins = new double[setCount];
            std::fill(setBegins, setBegins + setCount, 0);
        }

        /**
         * if hotTimes[point] == -1, we will use hotTime
         * relaxTime analogically
         */
        std::fill(hotTimes, hotTimes + setCount, hotTime);
        std::fill(relaxTimes, relaxTimes + setCount, relaxTime);
    }

    void setSetSize(const size_t i, const double _setSize) const {
        assert(i < setCount);
        setSizes[i] = _setSize;
    }

    void setHotProb(const size_t i, const double _hotProb) const {
        assert(i < setCount);
        hotProbs[i] = _hotProb;
    }

    void setHotTime(const size_t i, const int _hotTime) const {
        assert(i < setCount);
//        assert(hotTime == -1);
//
//        if (hotTimes == nullptr) {
//            hotTimes = new int[setCount];
//        }

        hotTimes[i] = _hotTime;
    }

    void setRelaxTimes(const size_t i, const int _relaxTime) const {
        assert(i < setCount);
//        assert(relaxTime == -1);
//
//        if (relaxTimes == nullptr) {
//            relaxTimes = new int[setCount];
//        }

        relaxTimes[i] = _relaxTime;
    }

    void setCommonHotTime(const int _hotTime) {
        hotTime = _hotTime;

        for (int i = 0; i < setCount; ++i) {
            if (hotTimes[i] == -1) {
                hotTimes[i] = hotTime;
            }
        }
    }

    void setCommonRelaxTime(const int _relaxTime) {
        relaxTime = _relaxTime;

        for (int i = 0; i < setCount; ++i) {
            if (relaxTimes[i] == -1) {
                relaxTimes[i] = relaxTime;
            }
        }
    }

    void setNotShuffle() {
        isNonShuffle = true;
        setBegins = new double[setCount];
        std::fill(setBegins, setBegins + setCount, 0);
    }

    void setSetBegin(const size_t i, const double _setBegin) {
        assert(i < setCount);
        setBegins[i] = _setBegin;
    }

    virtual ~TemporarySkewedParameters() {
        delete[] setSizes;
        delete[] hotProbs;
        delete[] hotTimes;
        delete[] relaxTimes;
    }

    virtual void parseArg(ParseArgument * args) {
        if (strcmp(args->getCurrent(), "-set-count") == 0) {
            this->setSetCount(atoi(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-rt") == 0) {
            this->setCommonRelaxTime(atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-ht") == 0) {
            this->setCommonHotTime(atof(args->getNext()));
//        } else if (std::regex_match(args->getCurrent(), std::regex(R"(ht\d+)"))) {
//            TSParm.setHotTime(atof(args->getCurrent().substr(2, 4)), atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-si") == 0) {
            int pointer = atoi(args->getNext());
            this->setSetSize(pointer, atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-pi") == 0) {
            int pointer = atoi(args->getNext());
            this->setHotProb(pointer, atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-hti") == 0) {
            int pointer = atoi(args->getNext());
            this->setHotTime(pointer, atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-rti") == 0) {
            int pointer = atoi(args->getNext());
            this->setRelaxTimes(pointer, atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-non-shuffle") == 0) {
            this->setNotShuffle();
        } else if (this->isNonShuffle && strcmp(args->getCurrent(), "-sbi") == 0) {
            int pointer = atoi(args->getNext());
            this->setSetBegin(pointer, atof(args->getNext()));
        } else {
            Parameters::parseArg(args);
        }
    }

};

#endif //SETBENCH_TEMPORARY_SKEWED_PARAMETERS_H
