//
// Created by Ravil Galiev on 02.09.2022.
//

#ifndef SETBENCH_OPS_PARAMETERS_H
#define SETBENCH_OPS_PARAMETERS_H

#include "common.h"
//#include "builder/distribution_builder.h"

struct OpsParameters : public Parameters {
    DistributionBuilder *readDistBuilder = new DistributionBuilder();
    DistributionBuilder *insertDistBuilder = new DistributionBuilder();
    DistributionBuilder *removeDistBuilder = new DistributionBuilder();
    bool prefill_sequential = false;

    ~OpsParameters() {
        delete readDistBuilder;
        delete insertDistBuilder;
        delete removeDistBuilder;
    }


    virtual std::string toString() {
        std::string params = Parameters::toString();

        params += "READ DISTRIBUTION             : " +
                  distributionTypeToString(readDistBuilder->distributionType) + "\n";
        params += readDistBuilder->toStringBuilderParameters();
        params += "INSERT DISTRIBUTION           : " +
                  distributionTypeToString(insertDistBuilder->distributionType) + "\n";
        params += insertDistBuilder->toStringBuilderParameters();
        params += "REMOVE DISTRIBUTION           : " +
                  distributionTypeToString(removeDistBuilder->distributionType) + "\n";
        params += removeDistBuilder->toStringBuilderParameters();

        if (prefill_sequential)
            params += "PREFILL TYPE                  : SEQUENTIAL\n";
        return params;
    }


    virtual void parseArg(ParseArgument * args) {
        if (strcmp(args->getCurrent(), "-prefill-sequential") == 0) {
            this->prefill_sequential = true;
        } else if (strcmp(args->getCurrent(), "-read-dist") == 0) {
            this->readDistBuilder->parse(args->next());
        } else if (strcmp(args->getCurrent(), "-insert-dist") == 0) {
            this->insertDistBuilder->parse(args->next());
        } else if (strcmp(args->getCurrent(), "-remove-dist") == 0) {
            this->removeDistBuilder->parse(args->next());
        } else {
            Parameters::parseArg(args);
        }
    }
};


#endif //SETBENCH_OPS_PARAMETERS_H
