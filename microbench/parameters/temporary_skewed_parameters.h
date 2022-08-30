//
// Created by Ravil Galiev on 25.08.2022.
//

#ifndef SETBENCH_TEMPORARY_SKEWED_PARAMETERS_H
#define SETBENCH_TEMPORARY_SKEWED_PARAMETERS_H


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

struct TemporarySkewedParameters {
public:
    int setCount;
    double *setSizes;
    double *hotProbs;
    int *hotTimes;
    int *relaxTimes;
    int hotTime = -1;
    int relaxTime = -1;

    void setSetCount(const int _setCount) {
        setCount = _setCount;
        setSizes = new double[setCount];
        hotProbs = new double[setCount];
        hotTimes = new int[setCount];
        relaxTimes = new int[setCount];

        /**
         * if hotTimes[i] == -1, we will use hotTime
         * relaxTime analogically
         */
        std::fill(hotTimes, hotTimes + setCount, -1);
        std::fill(relaxTimes, relaxTimes + setCount, -1);
    }

    void setSetSize(const int i, const double _setSize) {
        assert(i < setCount);
        setSizes[i] = _setSize;
    }

    void setHotProb(const int i, const double _hotProb) {
        assert(i < setCount);
        hotProbs[i] = _hotProb;
    }

    void setHotTime(const int i, const int _hotTime) {
        assert(i < setCount);
//        assert(hotTime == -1);
//
//        if (hotTimes == nullptr) {
//            hotTimes = new int[setCount];
//        }

        hotTimes[i] = _hotTime;
    }

    void setRelaxTimes(const int i, const int _relaxTime) {
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

    ~TemporarySkewedParameters() {
        delete[] setSizes;
        delete[] hotProbs;
        delete[] hotTimes;
        delete[] relaxTimes;
    }

};

#endif //SETBENCH_TEMPORARY_SKEWED_PARAMETERS_H
