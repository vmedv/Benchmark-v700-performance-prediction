//
// Created by Ravil Galiev on 31.07.2023.
//

#ifndef SETBENCH_DISTRIBUTION_JSON_CONVECTOR_H
#define SETBENCH_DISTRIBUTION_JSON_CONVECTOR_H

#include "distribution_builder.h"
#include "workloads/distributions/builders/biased_uniform_distribution_builder.h"
#include "workloads/distributions/builders/uniform_distribution_builder.h"
#include "workloads/distributions/builders/zipfian_distribution_builder.h"
#include "workloads/distributions/builders/skewed_uniform_distribution_builder.h"
#include "errors.h"

DistributionBuilder *getDistributionFromJson(const nlohmann::json &j) {
    std::string className = j["ClassName"];
    DistributionBuilder *distribution_builder;
    if (className == "UniformDistributionBuilder") {
        distribution_builder = new UniformDistributionBuilder();
    } else if (className == "ZipfianDistributionBuilder") {
        distribution_builder = new ZipfianDistributionBuilder();
    } else if (className == "SkewedUniformDistributionBuilder") {
        distribution_builder = new SkewedUniformDistributionBuilder();
    } else if (className == "BiasedUniformDistributionBuilder") {
        distribution_builder = new BiasedUniformDistributionBuilder();
    } else {
        setbench_error("JSON PARSER: Unknown class name DistributionBuilder -- " + className)
    }

    distribution_builder->fromJson(j);
    return distribution_builder;
}

MutableDistributionBuilder *getMutableDistributionFromJson(const nlohmann::json &j) {
    return dynamic_cast<MutableDistributionBuilder *>(getDistributionFromJson(j));
}

#endif //SETBENCH_DISTRIBUTION_JSON_CONVECTOR_H
