#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include "errors.h"
#include "workloads/args_generators/args_generator.h"
#include "workloads/args_generators/args_generator_builder.h"
#include "workloads/data_maps/builders/id_data_map_builder.h"
#include "workloads/data_maps/data_map.h"
#include "workloads/data_maps/data_map_builder.h"
#include "workloads/distributions/builders/uniform_distribution_builder.h"
#include "workloads/distributions/builders/biased_uniform_distribution_builder.h"
#include "workloads/distributions/distribution.h"

class SimpleSkewedArgsGenerator : public ArgsGenerator<int64_t> {
private:
    Distribution* query_;
    Distribution* hot_;
    Distribution* cold_;
    DataMap<int64_t>* data_;
    size_t hot_keys_;
    size_t range_;

    int64_t genArg() {
			// hot_keys := range * y%
        auto q_type = query_->next();
        int64_t d;
        if (q_type < hot_keys_) {
            d = hot_->next();
        } else {
            d = cold_->next();
        }

        return data_->get(d);
    }

public:
    SimpleSkewedArgsGenerator(size_t hot_keys, size_t range, Distribution* query, Distribution* hot,
                              Distribution* cold, DataMap<int64_t>* data)
        : query_(query),



          hot_(hot),
          cold_(cold),
          data_(data),
          hot_keys_(hot_keys),
          range_(range) {
    }

    int64_t nextGet() override {
        return genArg();
    }
    int64_t nextInsert() override {
        return genArg();
    }
    int64_t nextRemove() override {
        return genArg();
    }

    std::pair<int64_t, int64_t> nextRange() override {
        setbench_error("next range unsupported");
    }
};

class SimpleSkewedArgsGeneratorBuilder : public ArgsGeneratorBuilder {
private:
    size_t range_ = 0;
    size_t hot_ = 0;

public:
    ArgsGeneratorBuilder* init(size_t range) override {
        range_ = range;
        return this;
    }
    ArgsGenerator<long long>* build(Random64& rng) override {
        UniformDistributionBuilder b;
        BiasedUniformDistributionBuilder bb;
        return new SimpleSkewedArgsGenerator(hot_, range_, b.build(rng, range_), b.build(rng, hot_),
                                             bb.build(rng, range_ - hot_, hot_),
                                             IdDataMapBuilder().build());
    }
    void toJson(nlohmann::json& j) const override {
        j["ClassName"] = "SimpleSkewedArgsGeneratorBuilder";
        j["hot"] = hot_;
        j["range"] = range_;
    }
    void fromJson(const nlohmann::json& j) override {
        hot_ = j["hot"];
    }
    std::string toString(std::size_t) override {
        return {};
    }
};
