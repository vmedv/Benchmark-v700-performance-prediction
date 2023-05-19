//
// Created by Ravil Galiev on 25.08.2022.
//

#ifndef SETBENCH_TEMPORARY_WAVE_SPECIAL_REMOVE_PARAMETERS_H
#define SETBENCH_TEMPORARY_WAVE_SPECIAL_REMOVE_PARAMETERS_H

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

struct TemporaryWaveSpecialRemoveParameters : public Parameters {
    size_t setCount = 0;
    int *hotTimes;
    int hotTime = -1;

    double *setBegins;

    DistributionBuilder *waveDistBuilder = (new DistributionBuilder(
            DistributionType::ZIPF))->setParameters(new ZipfParameters(1));
    DistributionBuilder *removeDistBuilder = new DistributionBuilder();
    DistributionBuilder *prefillDistBuilder = new DistributionBuilder();


    virtual std::string toString() {
        std::string params = Parameters::toString();


        params += "KEY_GENERATOR                 : TEMPORARY_WAVE_SPECIAL_REMOVE\n";
        params += "NUMBER_OF_SETS                : " + std::to_string(setCount) + "\n";


        params += "COMMON_HOT_TIME               : " + std::to_string(hotTime) + "\n";
        params += "HOT_TIMES:\n";
        for (int i = 0; i < setCount; i++) {
            params += "    HOT_TIMES_" + std::to_string(i) + "               : "
                      + std::to_string(hotTimes[i]) + "\n";
        }

        params += "WAVE_DISTRIBUTION             : "
                  + distributionTypeToString(waveDistBuilder->distributionType) + "\n";
        params += waveDistBuilder->toStringBuilderParameters();

        params += "REMOVE_DISTRIBUTION           : "
                  + distributionTypeToString(removeDistBuilder->distributionType) + "\n";
        params += removeDistBuilder->toStringBuilderParameters();

        params += "PREFILL_DISTRIBUTION          : "
                  + distributionTypeToString(prefillDistBuilder->distributionType) + "\n";
        params += prefillDistBuilder->toStringBuilderParameters();

        params += "SETS_BEGINS:\n";
        for (int i = 0; i < setCount; i++) {
            params += "    SET_BEGIN_" + std::to_string(i) + "               : "
                      + std::to_string(setBegins[i]) + "\n";
        }

        return params;
    }

    void setSetCount(const size_t _setCount) {
        setCount = _setCount;
        hotTimes = new int[setCount];

        setBegins = new double[setCount];
        std::fill(setBegins, setBegins + setCount, 0);

        /**
         * if hotTimes[point] == -1, we will use hotTime
         */
        std::fill(hotTimes, hotTimes + setCount, hotTime);
    }

    void setHotTime(const size_t i, const int _hotTime) const {
        assert(i < setCount);
        hotTimes[i] = _hotTime;
    }

    void setCommonHotTime(const int _hotTime) {
        hotTime = _hotTime;

        for (int i = 0; i < setCount; ++i) {
            if (hotTimes[i] == -1) {
                hotTimes[i] = hotTime;
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

    virtual ~TemporaryWaveSpecialRemoveParameters() {
        delete[] hotTimes;
        delete waveDistBuilder;
        delete removeDistBuilder;
        delete prefillDistBuilder;
    }

    virtual void parseArg(ParseArgument *args) {
        if (strcmp(args->getCurrent(), "-set-count") == 0) {
            this->setSetCount(atoi(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-ht") == 0) {
            this->setCommonHotTime(atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-hti") == 0) {
            int pointer = atoi(args->getNext());
            this->setHotTime(pointer, atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-non-shuffle") == 0) {
            this->setNotShuffle();
        } else if (strcmp(args->getCurrent(), "-sbi") == 0) {
            int pointer = atoi(args->getNext());
            this->setSetBegin(pointer, atof(args->getNext()));
        } else if (strcmp(args->getCurrent(), "-wave-dist") == 0
                   || strcmp(args->getCurrent(), "-w-dist") == 0) {
            this->waveDistBuilder->parse(args->next());
        } else if (strcmp(args->getCurrent(), "-remove-dist") == 0
                   || strcmp(args->getCurrent(), "-r-dist") == 0) {
            this->removeDistBuilder->parse(args->next());
        } else if (strcmp(args->getCurrent(), "-prefill-dist") == 0
                   || strcmp(args->getCurrent(), "-p-dist") == 0) {
            this->prefillDistBuilder->parse(args->next());
        } else {
            Parameters::parseArg(args);
        }
    }

};

#endif //SETBENCH_TEMPORARY_WAVE_SPECIAL_REMOVE_PARAMETERS_H
