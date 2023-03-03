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

};

class CreakersAndWaveParametersParser : public ParametersParser {
protected:
    CreakersAndWaveParameters *localParameters;

    virtual void parseArg() {
        if (strcmp(argv[point], "-gs") == 0
            || strcmp(argv[point], "-cs") == 0) {
            localParameters->CREAKERS_SIZE = atof(argv[++point]);
        } else if (strcmp(argv[point], "-gp") == 0 || strcmp(argv[point], "-cp") == 0) {
            localParameters->CREAKERS_PROB = atof(argv[++point]);
        } else if (strcmp(argv[point], "-ws") == 0) {
            localParameters->WAVE_SIZE = atof(argv[++point]);
        } else if (strcmp(argv[point], "-g-age") == 0 || strcmp(argv[point], "-c-age") == 0) {
            localParameters->CREAKERS_AGE = atoi(argv[++point]);
        } else if (strcmp(argv[point], "-g-dist") == 0 || strcmp(argv[point], "-c-dist") == 0) {
            localParameters->creakersDistBuilder->parse(argc, argv, point);
        } else if (strcmp(argv[point], "-w-dist") == 0) {
            localParameters->waveDistBuilder->parse(argc, argv, point);
        } else if (strcmp(argv[point], "-prefillsize") == 0) {
            ++point;
            std::cout << "CreakersAndWave key generator does not accept prefill size argument. Ignoring...\n";
        } else {
            ParametersParser::parseArg();
        }
    }

public:
    CreakersAndWaveParametersParser(size_t _argc, char **_argv, size_t _point = 0) {
        argc = _argc;
        argv = _argv;
        point = _point;

        localParameters = new CreakersAndWaveParameters();
        parameters = localParameters;
    }

    void parse() {
        ParametersParser::parse();
        parameters->DESIRED_PREFILL_SIZE = (int) (parameters->MAXKEY * localParameters->CREAKERS_SIZE) +
                                           (int) (parameters->MAXKEY * localParameters->WAVE_SIZE);
    }

};

#endif //SETBENCH_CREAKERS_AND_WAVE_PARAMETERS_H
