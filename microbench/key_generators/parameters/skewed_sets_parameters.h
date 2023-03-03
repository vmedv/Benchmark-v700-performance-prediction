//
// Created by Ravil Galiev on 26.08.2022.
//

#ifndef SETBENCH_SKEWED_SETS_PARAMETERS_H
#define SETBENCH_SKEWED_SETS_PARAMETERS_H

#include "common.h"

struct SkewedSetsParameters : public Parameters {
    double READ_HOT_SIZE = 0;
    double READ_HOT_PROB = 0;
    double WRITE_HOT_SIZE = 0;
    double WRITE_HOT_PROB = 0;
    double INTERSECTION = 0;
    bool writePrefillOnly = false;


    virtual std::string toString() {
        std::string params = Parameters::toString();

        params += "KEY_GENERATOR                 : SKEWED_SETS\n";
        params += "READ_HOT_SIZE                 : " + std::to_string(READ_HOT_SIZE) + "\n";
        params += "WRITE_HOT_SIZE                : " + std::to_string(WRITE_HOT_SIZE) + "\n";
        params += "INTERSECTION                  : " + std::to_string(INTERSECTION) + "\n";
        params += "READ_HOT_PROBABILITY          : " + std::to_string(READ_HOT_PROB) + "\n";
        params += "WRITE_HOT_PROBABILITY         : " + std::to_string(WRITE_HOT_PROB) + "\n";

        if (writePrefillOnly) {
            params += "PREFFIL_TYPE                  : WRITE_ONLY\n";
        }

        return params;
    }
};

class SkewedSetsParametersParser : public ParametersParser {
protected:
    SkewedSetsParameters *localParameters;

    virtual void parseArg() {
        if (strcmp(argv[point], "-rs") == 0) {
            localParameters->READ_HOT_SIZE = atof(argv[++point]);
        } else if (strcmp(argv[point], "-rp") == 0) {
            localParameters->READ_HOT_PROB = atof(argv[++point]);
        } else if (strcmp(argv[point], "-ws") == 0) {
            localParameters->WRITE_HOT_SIZE = atof(argv[++point]);
        } else if (strcmp(argv[point], "-wp") == 0) {
            localParameters->WRITE_HOT_PROB = atof(argv[++point]);
        } else if (strcmp(argv[point], "-inter") == 0) {
            localParameters->INTERSECTION = atof(argv[++point]);
        } else if (strcmp(argv[point], "-write-prefill-only") == 0) {
            localParameters->writePrefillOnly = true;
        } else {
            ParametersParser::parseArg();
        }

        //todo with skewed set distributions
    }

public:

    SkewedSetsParametersParser(size_t _argc, char **_argv, size_t _point = 0) {
        argc = _argc;
        argv = _argv;
        point = _point;

        localParameters = new SkewedSetsParameters();
        parameters = localParameters;
    }
};

#endif //SETBENCH_SKEWED_SETS_PARAMETERS_H
