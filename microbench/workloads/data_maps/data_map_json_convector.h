//
// Created by Ravil Galiev on 27.07.2023.
//

#ifndef SETBENCH_DATA_MAP_JSON_CONVECTOR_H
#define SETBENCH_DATA_MAP_JSON_CONVECTOR_H


#include "json/single_include/nlohmann/json.hpp"
#include "data_map_builder.h"
#include "workloads/data_maps/builders/id_data_map_builder.h"
#include "workloads/data_maps/builders/array_data_map_builder.h"

DataMapBuilder *getDataMapFromJson(const nlohmann::json &j) {
    DataMapType type = j["dataMapType"];
    DataMapBuilder * dataMapBuilder;
    switch (type) {
        case DataMapType::ID:
            dataMapBuilder = new IdDataMapBuilder();
            break;
        case DataMapType::ARRAY:
            dataMapBuilder = new ArrayDataMapBuilder();
            break;
        case DataMapType::HASH:
            break;
    }
    dataMapBuilder->fromJson(j);
    return dataMapBuilder;
}

#endif //SETBENCH_DATA_MAP_JSON_CONVECTOR_H
