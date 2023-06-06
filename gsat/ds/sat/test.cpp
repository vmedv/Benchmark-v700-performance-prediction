#include "catch.hpp"

#include "stress_test.h"
#include "unit_test.h"

#include "tree_builder.h"

#include "sat.h"

template<typename K, typename V>
class SATBuilder : public TreeBuilder<SAT<K, V>, K, V> {
public:
    using Tree = SAT<K, V>;
    using Base = TreeBuilder<SAT<K, V>, K, V>;

    Tree *Build() override {
        return new Tree(*Base::no_value_);
    }
};

TEST_CASE("sat_small_stress") {
    tree_tests::MakeSmallStressTest(new SATBuilder<int, int>())();
}

TEST_CASE("sat_big_stress") {
    tree_tests::MakeMidStressTest(new SATBuilder<int, int>())();
}

TEST_CASE("sat_insert_delete") {
    tree_tests::MakeInsertDeleteUnitTest(new SATBuilder<int, int>())();
}
