//
// Created by Ravil Galiev on 24.07.2023.
//

#ifndef SETBENCH_DATA_MAP_BUILDER_H
#define SETBENCH_DATA_MAP_BUILDER_H

#include <string>
#include "data_map.h"
#include "json/single_include/nlohmann/json.hpp"

typedef long long K;

//template<typename K>
class DataMapBuilder {
    static size_t idCounter;
    DataMap<K> *dataMap = nullptr;
public:
    size_t id = idCounter++;

    virtual DataMapBuilder *init(size_t range) = 0;

    virtual DataMap<K> *build() = 0;

    DataMap<K> *getOrBuild() {
        if (dataMap == nullptr) {
            dataMap = build();
        }
        return dataMap;
    }

    virtual std::string toString(size_t indents = 1) = 0;

    virtual void toJson(nlohmann::json &j) const = 0;

    virtual void fromJson(const nlohmann::json &j) = 0;

    virtual ~DataMapBuilder() = default;
};

size_t DataMapBuilder::idCounter = 0;

void to_json(nlohmann::json &j, const DataMapBuilder &s) {
    s.toJson(j);
    j["id"] = s.id;
    assert(j["dataMapType"] != nullptr);
}

void from_json(const nlohmann::json &j, DataMapBuilder &s) {
    s.fromJson(j);
}

enum class DataMapType {
    ID, ARRAY, HASH
};

#endif //SETBENCH_DATA_MAP_BUILDER_H
