//
// Created by Ravil Galiev on 29.08.2022.
//

#ifndef SETBENCH_CREAKERS_AND_WAVE_PARAMETERS_H
#define SETBENCH_CREAKERS_AND_WAVE_PARAMETERS_H


#include "parameters/key_gen_distribution_type.h"

/**
    старички + волна
    n — g — gx — gk — cp — ca
        n — количество элементов
        g (grand) — процент старичков
            // 100% - g — процент новичков
        gy — вероятность их вызова
            // 100% - gx — вероятность вызова новичков
        gk — на сколько стары наши старички
            | преподсчёт - перед началом теста делаем gk количество запросов к старичкам
        cp (child) — распределение вызовов среди новичков
            // по умолчанию Zipf 1
            // при желании можно сделать cx — cy
        ca (child add) — вероятность добавления нового элемента
            // 100% - gx - ca — чтение новичка, ca добавление нового новичка
            // при желании можно переработать на "100% - ca — чтение, ca — запись"
                (то есть брать ca не от общей вероятности, а только при чтении"
 */

struct CreakersAndWaveParameters {
public:

    double CREAKERS_SIZE = 0;
    double CREAKERS_PROB = 0;
    size_t CREAKERS_AGE = 0;
    double WAVE_SIZE = 0;
    KeyGeneratorDistribution grandDist = UNIFORM;
    KeyGeneratorDistribution newWaveDist = MUTABLE_ZIPF;

};

#endif //SETBENCH_CREAKERS_AND_WAVE_PARAMETERS_H
