//
// Created by Ravil Galiev on 02.09.2022.
//

#ifndef SETBENCH_SIMPLE_PARAMETERS_H
#define SETBENCH_SIMPLE_PARAMETERS_H

#include "common.h"
//#include "builder/distribution_builder.h"

struct SimpleParameters : public Parameters {
    DistributionBuilder *distributionBuilder = new DistributionBuilder();
    bool prefill_sequential = false;

    ~SimpleParameters() {
        delete distributionBuilder;
    }


    virtual std::string toString() {
        std::string params = Parameters::toString();

        params += "DISTRIBUTION                  : " + distributionTypeToString(distributionBuilder->distributionType) +
                  "\n";
        params += distributionBuilder->toStringBuilderParameters();
        if (prefill_sequential)
            params += "PREFILL TYPE                  : SEQUENTIAL\n";
        return params;
    }


    virtual void parseArg(ParseArgument * args) {
        if (strcmp(args->getCurrent(), "-prefill-sequential") == 0) {
            this->prefill_sequential = true;
        } else if (!this->distributionBuilder->parse(args)) {
            Parameters::parseArg(args);
        }
    }
};


#endif //SETBENCH_SIMPLE_PARAMETERS_H
