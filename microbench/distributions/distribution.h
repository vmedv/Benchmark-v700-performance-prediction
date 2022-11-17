//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_DISTRIBUTION_H
#define SETBENCH_DISTRIBUTION_H

enum class DistributionType {
    UNIFORM, ZIPF, ZIPF_FAST, MUTABLE_ZIPF, SKEWED_SETS
};

char * distributionTypeToString(DistributionType distributionType) {

    switch (distributionType) {
        case DistributionType::UNIFORM:
            return "UNIFORM";
        case DistributionType::ZIPF:
            return "ZIPF";
        case DistributionType::ZIPF_FAST:
            return "ZIPF_FAST";
        case DistributionType::MUTABLE_ZIPF:
            return "MUTABLE_ZIPF";
        case DistributionType::SKEWED_SETS:
            break;
    }
}

class Distribution {
public:
    Distribution() = default;

    virtual size_t next() = 0;
};

class MutableDistribution : public Distribution {
public:
    MutableDistribution() = default;

    virtual void setMaxKey(size_t _maxKey) = 0;

    virtual size_t next(size_t _maxKey) = 0;
};


#endif //SETBENCH_DISTRIBUTION_H
