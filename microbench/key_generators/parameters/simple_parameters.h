//
// Created by Ravil Galiev on 02.09.2022.
//

#ifndef SETBENCH_SIMPLE_PARAMETERS_H
#define SETBENCH_SIMPLE_PARAMETERS_H

#include "common.h"
//#include "builder/distribution_builder.h"

struct SimpleParameters : public Parameters {
    DistributionBuilder *distributionBuilder = new DistributionBuilder();

    ~SimpleParameters() {
        delete distributionBuilder;
    }


    virtual std::string toString() {
        std::string params = Parameters::toString();

        params += "DISTRIBUTION                  : " + distributionTypeToString(distributionBuilder->distributionType) +
                  "\n";
        params += distributionBuilder->toStringBuilderParameters();

        return params;
    }
};

class SimpleParametersParser : public ParametersParser {
protected:
    SimpleParameters *localParameters;

    virtual void parseArg() {
        size_t oldPoint = point;
        localParameters->distributionBuilder->parse(argc, argv, point);

        if (point == oldPoint) {
            ParametersParser::parseArg();
        }
    }

public:
    SimpleParametersParser(size_t _argc, char **_argv, size_t _point = 0) {
        argc = _argc;
        argv = _argv;
        point = _point;

        localParameters = new SimpleParameters();
        parameters = localParameters;
    }
};

#endif //SETBENCH_SIMPLE_PARAMETERS_H
