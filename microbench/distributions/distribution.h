//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_DISTRIBUTION_H
#define SETBENCH_DISTRIBUTION_H

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
