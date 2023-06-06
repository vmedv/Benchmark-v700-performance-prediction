#include "catch.hpp"

#include "stress_test.h"
#include "unit_test.h"

#include "tree_builder.h"

#include "sabt.h"

template <typename K, typename V, int kMinKeys>
class SABTBuilder : public TreeBuilder<SABT<K, V, kMinKeys>, K, V> {
public:
    using Tree = SABT<K, V, kMinKeys>;
    using Base = TreeBuilder<SABT<K, V, kMinKeys>, K, V>;

    Tree* Build() override {
        return new Tree(*Base::no_value_);
    }
};

TEST_CASE("sabt<3>_small_stress") {
    tree_tests::MakeSmallStressTest(new SABTBuilder<int, int, 3>())();
}

TEST_CASE("sabt<7>_big_stress") {
    tree_tests::MakeMidStressTest(new SABTBuilder<int, int, 7>())();
}

TEST_CASE("sabt<4>_insert_delete") {
    tree_tests::MakeInsertDeleteUnitTest(new SABTBuilder<int, int, 4>())();
}
