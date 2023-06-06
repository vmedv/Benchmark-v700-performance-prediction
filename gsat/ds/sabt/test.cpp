#include "catch.hpp"

#include <stress_test.h>
#include <unit_test.h>
#include <tree_builder.h>

#include "sabt.h"

template<typename Value, int kMinKeys>
class SABTBuilder : public TreeBuilder<SABT<int, Value, kMinKeys>, Value> {
public:
    using Tree = SABT<int, Value, kMinKeys>;

    SABTBuilder(int left, int right) : left_(left), right_(right) {}

    Tree *Build() override {
        return new Tree(this->GetNoValue(), left_, right_);
    }

private:
    int left_;
    int right_;
};

template<int kMinKeys>
void StressTest(const tree_tests::StressTestConfig<int> &config) {
    tree_tests::StressTest(new SABTBuilder<tree_tests::ValueType, kMinKeys>(config.GetMinKey(), config.GetMaxKey()), config)();
}

TEST_CASE("insert_delete") {
    tree_tests::UnitTest(new SABTBuilder<int, 3>(-100, 100), tree_tests::kInsertDeleteAction)();
}

TEST_CASE("small_stress") {
    StressTest<3>(tree_tests::kSmallStressTestConfig);
}
