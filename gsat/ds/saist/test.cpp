#include "catch.hpp"

#include "stress_test.h"
#include "unit_test.h"

#include "tree_builder.h"

#include "saist.h"

template <typename K, typename V>
class SAISTBuilder : public TreeBuilder<SAIST<K, V>, K, V> {
public:
    using Tree = SAIST<K, V>;
    using Base = TreeBuilder<SAIST<K, V>, K, V>;

    SAISTBuilder(double alpha)
        : alpha_(alpha) {
    }

    Tree* Build() override {
        return new Tree(*Base::no_value_, alpha_, -1e9, 1e9);
    }

private:
    double alpha_;
};

TEST_CASE("saist<0.5>_small_stress") {
    tree_tests::MakeSmallStressTest(new SAISTBuilder<int, int>(0.5))();
}

TEST_CASE("saist<0.5>_big_stress") {
    tree_tests::MakeMidStressTest(new SAISTBuilder<int, int>(0.5))();
}

TEST_CASE("saist<0.25>_insert_delete") {
    tree_tests::MakeInsertDeleteUnitTest(new SAISTBuilder<int, int>(0.25))();
}
