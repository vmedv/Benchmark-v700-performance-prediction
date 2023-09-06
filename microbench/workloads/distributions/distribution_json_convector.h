//
// Created by Ravil Galiev on 31.07.2023.
//

#ifndef SETBENCH_DISTRIBUTION_JSON_CONVECTOR_H
#define SETBENCH_DISTRIBUTION_JSON_CONVECTOR_H

#include "distribution_builder.h"
#include "workloads/distributions/builders/uniform_distribution_builder.h"
#include "workloads/distributions/builders/zipf_distribution_builder.h"
#include "workloads/distributions/builders/skewed_uniform_distribution_builder.h"

DistributionBuilder *getDistributionFromJson(const nlohmann::json &j) {
    DistributionType type = j["distributionType"];
    DistributionBuilder * distributionBuilder;
    switch (type) {
        case DistributionType::UNIFORM:
            distributionBuilder = new UniformDistributionBuilder();
            break;
        case DistributionType::ZIPF:
            distributionBuilder = new ZipfDistributionBuilder();
            break;
        case DistributionType::SKEWED_UNIFORM:
            distributionBuilder = new SkewedUniformDistributionBuilder();
            break;
    }
    distributionBuilder->fromJson(j);
    return distributionBuilder;
}

MutableDistributionBuilder *getMutableDistributionFromJson(const nlohmann::json &j) {
    return dynamic_cast<MutableDistributionBuilder *>(getDistributionFromJson(j));
}

#endif //SETBENCH_DISTRIBUTION_JSON_CONVECTOR_H
