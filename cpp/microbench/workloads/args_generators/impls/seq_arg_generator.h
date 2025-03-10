#include <cstddef>
#include <string>
#include "workloads/args_generators/args_generator.h"
#include "workloads/args_generators/args_generator_builder.h"
#include <errors.h>

class SeqArgGenerator : public ArgsGenerator<long long> {
private:
    size_t range_ = 0;

    long long get_idx_ = 0;
    long long insert_idx_ = 0;
    long long remove_idx_ = 0;
    long long range_idx_ = 0;

public:
    explicit SeqArgGenerator(size_t range)
        : range_(range) {
    }

    long long nextGet() override {
        return get_idx_++ % range_ + 1;
    }
    long long nextInsert() override {
        return insert_idx_++ % range_ + 1;
    }
    long long nextRemove() override {
        return remove_idx_++ % range_ + 1;
    }

    std::pair<long long, long long> nextRange() override {
        return {range_idx_ % range_, ++range_idx_ % range_};
    }
};

class SeqArgGeneratorBuilder : public ArgsGeneratorBuilder {
private:
    size_t range_ = 0;

public:
    ArgsGeneratorBuilder* init(size_t range) override {
        range_ = range;
        return this;
    }
    ArgsGenerator<long long>* build(Random64&) override {
        return new SeqArgGenerator(range_);
    }
    void toJson(nlohmann::json& j) const override {
        j["ClassName"] = "SeqArgsGeneratorBuilder";
    }
    void fromJson(const nlohmann::json& j) override {
    }
    std::string toString(std::size_t) override {
        return {};
    }
};
