//
// Created by Ravil Galiev on 30.08.2022.
//

#ifndef SETBENCH_DISTRIBUTION_H
#define SETBENCH_DISTRIBUTION_H

template<typename K>
class Distribution {
public:
    Distribution() = default;

    virtual K next() = 0;
};


#endif //SETBENCH_DISTRIBUTION_H
