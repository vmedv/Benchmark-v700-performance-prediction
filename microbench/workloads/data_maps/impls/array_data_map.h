//
// Created by Ravil Galiev on 24.07.2023.
//

#ifndef SETBENCH_ARRAY_DATA_MAP_H
#define SETBENCH_ARRAY_DATA_MAP_H

#include <algorithm>
#include "workloads/data_maps/data_map.h"

class ArrayDataMap : public DataMap<long long> {
private:
    long long *data;
public:

    ArrayDataMap(size_t range) {
        data = new long long[range];
        for (size_t i = 0; i < range; i++) {
            data[i] = i + 1;
        }

        std::random_shuffle(data, data + range - 1);
    }

    long long get(size_t index) override {
        return data[index];
    }

    ~ArrayDataMap() {
        delete[] data;
    }

};

#endif //SETBENCH_ARRAY_DATA_MAP_H
