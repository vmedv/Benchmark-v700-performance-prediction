//
// Created by Ravil Galiev on 29.08.2022.
//

#ifndef SETBENCH_CREAKERS_AND_WAVE_PARAMETERS_H
#define SETBENCH_CREAKERS_AND_WAVE_PARAMETERS_H

#include "common.h"
#include "distributions/builder/distribution_builder.h"

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

struct CreakersAndWaveParameters : public Parameters {
    double CREAKERS_SIZE = 0;
    double CREAKERS_PROB = 0;
    size_t CREAKERS_AGE = 0;
    double WAVE_SIZE = 0;
    DistributionBuilder *creakersDistBuilder = new DistributionBuilder();
    DistributionBuilder *waveDistBuilder = (new DistributionBuilder(
            DistributionType::ZIPF))->setParameters(new ZipfParameters(1));

    virtual ~CreakersAndWaveParameters() {
        delete creakersDistBuilder;
        delete waveDistBuilder;
    }

    virtual std::string toString() {
        std::string params = Parameters::toString();


        params += "KEY_GENERATOR                 : CREAKERS_AND_WAVE\n";
        params += "CREAKERS_SIZE                 : " + std::to_string(CREAKERS_SIZE) + "\n";
        params += "WAVE_SIZE                     : " + std::to_string(WAVE_SIZE) + "\n";
        params += "CREAKERS_PROBABILITY          : " + std::to_string(CREAKERS_PROB) + "\n";
        params += "CREAKERS_AGE                  : " + std::to_string(CREAKERS_AGE) + "\n";
        params += "CREAKERS_DISTRIBUTION         : " + distributionTypeToString(creakersDistBuilder->distributionType) +
                  "\n";
        params += creakersDistBuilder->toStringBuilderParameters();
        params += "WAVE_DISTRIBUTION             : " + distributionTypeToString(waveDistBuilder->distributionType) +
                  "\n";
        params += waveDistBuilder->toStringBuilderParameters();

        return params;
    }

    virtual void parseArg(ParseArgument *args) {
        if (strcmp(args->getCurrent(), "-gs") == 0
            || strcmp(args->getCurrent(), "-cs") == 0) {
            this->CREAKERS_SIZE = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-gp") == 0 || strcmp(args->getCurrent(), "-cp") == 0) {
            this->CREAKERS_PROB = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-ws") == 0) {
            this->WAVE_SIZE = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-g-age") == 0 || strcmp(args->getCurrent(), "-c-age") == 0) {
            this->CREAKERS_AGE = atoi(args->getNext());
        } else if (strcmp(args->getCurrent(), "-g-dist") == 0 || strcmp(args->getCurrent(), "-c-dist") == 0) {
            this->creakersDistBuilder->parse(args->next());
        } else if (strcmp(args->getCurrent(), "-w-dist") == 0) {
            this->waveDistBuilder->parse(args->next());
        } else if (strcmp(args->getCurrent(), "-prefillsize") == 0) {
            args->getNext();
            std::cout << "CreakersAndWave key generator does not accept prefill size argument. Ignoring...\n";
        } else {
            Parameters::parseArg(args);
        }
    }

    virtual void build() {
        this->DESIRED_PREFILL_SIZE = (int) (this->MAXKEY * this->CREAKERS_SIZE) +
                                     (int) (this->MAXKEY * this->WAVE_SIZE);
    }

};

#endif //SETBENCH_CREAKERS_AND_WAVE_PARAMETERS_H
