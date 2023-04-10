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

    virtual void parseArg(ParseArgument * args) {
        if (strcmp(args->getCurrent(), "-rs") == 0) {
            this->READ_HOT_SIZE = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-rp") == 0) {
            this->READ_HOT_PROB = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-ws") == 0) {
            this->WRITE_HOT_SIZE = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-wp") == 0) {
            this->WRITE_HOT_PROB = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-inter") == 0) {
            this->INTERSECTION = atof(args->getNext());
        } else if (strcmp(args->getCurrent(), "-write-prefill-only") == 0) {
            this->writePrefillOnly = true;
        } else {
            Parameters::parseArg(args);
        }

        //todo with skewed set distributions
    }
};

#endif //SETBENCH_SKEWED_SETS_PARAMETERS_H
