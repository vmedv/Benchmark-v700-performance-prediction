//
// Created by Ravil Galiev on 28.02.2023.
//

#ifndef SETBENCH_DISTRIBUTION_BUILDER_H
#define SETBENCH_DISTRIBUTION_BUILDER_H

#include "common.h"

struct DistributionBuilder {
    DistributionType distributionType;
    DistributionParameters *parameters;

    DistributionBuilder() : distributionType(DistributionType::UNIFORM) {};

    DistributionBuilder(DistributionType _distributionType) : distributionType(_distributionType) {}

    DistributionBuilder *setType(DistributionType _distributionType) {
        distributionType = _distributionType;
        return this;
    }

    DistributionBuilder *setParameters(DistributionParameters *const _parameters) {
        parameters = _parameters;
        return this;
    }

    size_t parse(size_t &argc, char **argv, size_t &point);

    Distribution *getDistribution(Random64 *rng, size_t range);

    MutableDistribution *getMutableDistribution(Random64 *rng);

    std::string toStringBuilderParameters() {
        return parameters != nullptr ? parameters->toString() : "";
    }

    ~DistributionBuilder() {
        delete parameters;
    }

};

#include "distributions/parameters/distribution_parameters_impls.h"

size_t DistributionBuilder::parse(size_t &argc, char **argv, size_t &point) {
    if (strcmp(argv[point], "-dist-zipf") == 0) {
        setType(DistributionType::ZIPF);
        setParameters(new ZipfParameters(atof(argv[++point])));
    } else if (strcmp(argv[point], "-dist-skewed-sets") == 0) {
        setType(DistributionType::SKEWED_SETS);
        //todo add parameters parse
    } else if (strcmp(argv[point], "-dist-uniform") == 0) {
        setType(DistributionType::UNIFORM);
    }

    return point;
}

Distribution *DistributionBuilder::getDistribution(Random64 *rng, size_t range) {
    switch (this->distributionType) {
        case DistributionType::UNIFORM:
            return new UniformDistribution(rng, range);
        case DistributionType::ZIPF:
            return new ZipfDistribution(rng, ((ZipfParameters *) parameters)->alpha, range);
        case DistributionType::SKEWED_SETS:
            SkewedSetParameters *skewedSetParameters = (SkewedSetParameters *) parameters;
            return new SkewedSetsDistribution(
                    skewedSetParameters->hotDistBuilder->
                            getDistribution(rng, skewedSetParameters->getHotLength(range)),
                    skewedSetParameters->coldDistBuilder->
                            getDistribution(rng, skewedSetParameters->getColdLength(range)),
                    rng,
                    skewedSetParameters->hotProb,
                    skewedSetParameters->getHotLength(range)
            );
    }
}

MutableDistribution *DistributionBuilder::getMutableDistribution(Random64 *rng) {
    switch (this->distributionType) {
        case DistributionType::UNIFORM:
            return new UniformDistribution(rng);
        case DistributionType::ZIPF:
            return new ZipfDistribution(rng, ((ZipfParameters *) parameters)->alpha);
        default:
            return NULL;
    }
}

#endif //SETBENCH_DISTRIBUTION_BUILDER_H
