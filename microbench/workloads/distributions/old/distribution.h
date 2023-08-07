//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_DISTRIBUTION_H
#define SETBENCH_DISTRIBUTION_H

enum class DistributionType {
    UNIFORM, ZIPF, SKEWED_UNIFORM
};

std::string distributionTypeToString(DistributionType distributionType) {
    switch (distributionType) {
        case DistributionType::UNIFORM:
            return "UNIFORM";
        case DistributionType::ZIPF:
            return "ZIPF";
        case DistributionType::SKEWED_UNIFORM:
            return "SKEWED_SETS";
    }
}

struct Distribution {
    virtual size_t next() = 0;

    virtual ~Distribution() {}
};

struct MutableDistribution : public Distribution {
    virtual void setMaxKey(size_t _maxKey) = 0;

    virtual size_t next(size_t _maxKey) {
        setMaxKey(_maxKey);
        return Distribution::next();
    }

    virtual ~MutableDistribution() {};
};


#endif //SETBENCH_DISTRIBUTION_H
