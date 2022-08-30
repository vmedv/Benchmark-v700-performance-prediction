//
// Created by Ravil Galiev on 29.08.2022.
//

#ifndef SETBENCH_KEY_GEN_DISTRIBUTION_TYPE_H
#define SETBENCH_KEY_GEN_DISTRIBUTION_TYPE_H


enum KeyGeneratorDistribution {
    UNIFORM, ZIPF, ZIPFFAST, SKEWED_SETS, TEMPORARY_SKEWED, CREAKERS_AND_WAVE, MUTABLE_ZIPF;
};

//#include "distributions.h"
//
//template<typename K>
//Distribution<K> getDistribution(){
//    switch (this) {
//        case UNIFORM:
//            return new UniformDistribution<K>();
//        default:
//            return NULL;
//    }
//}

#endif //SETBENCH_KEY_GEN_DISTRIBUTION_TYPE_H
