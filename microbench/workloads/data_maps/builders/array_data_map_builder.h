//
// Created by Ravil Galiev on 24.07.2023.
//

#ifndef SETBENCH_ARRAY_DATA_MAP_BUILDER_H
#define SETBENCH_ARRAY_DATA_MAP_BUILDER_H

#include "workloads/data_maps/data_map_builder.h"
#include "workloads/data_maps/impls/id_data_map.h"
#include "workloads/data_maps/impls/array_data_map.h"

struct ArrayDataMapBuilder : public DataMapBuilder {
    size_t range;

    ArrayDataMapBuilder *init(size_t _range) override {
        range = _range;
        return this;
    };

    ArrayDataMap *build() override {
        return new ArrayDataMap(range);
    };

    void toJson(nlohmann::json &j) const override {
        j["dataMapType"] = DataMapType::ARRAY;
    }

    void fromJson(const nlohmann::json &j) override {

    }

    std::string toString(size_t indents = 1) override {
        return indented_title_with_str_data("Type", "ArrayDataMap", indents);
    }

    ~ArrayDataMapBuilder() override = default;
};

#endif //SETBENCH_ARRAY_DATA_MAP_BUILDER_H
