#include <catch2/catch.hpp>

#include <cbag/util/interval.h>

namespace cu = cbag::util;

using coord_t = cu::disjoint_intvs::coord_t;
using key_type = cu::disjoint_intvs::key_type;

SCENARIO("disjoint_intvs is empty", "[disjoint_intvs]") {
    cu::disjoint_intvs iset;
    REQUIRE(iset.size() == 0);
    REQUIRE(iset.empty() == true);

    THEN("start and stop errors") {
        REQUIRE_THROWS_AS(iset.start(), std::out_of_range);
        REQUIRE_THROWS_AS(iset.stop(), std::out_of_range);
    }

    THEN("complement works") {
        coord_t lower = 0;
        coord_t upper = 6;
        auto comp = iset.get_complement(lower, upper);
        REQUIRE(comp.start() == lower);
        REQUIRE(comp.stop() == upper);
        REQUIRE(comp.size() == 1);
    }

    WHEN("adding values") {
        bool success = iset.add(key_type(8, 10), nullptr);
        REQUIRE(success == true);
        success = iset.add(key_type(4, 6), nullptr);
        REQUIRE(success == true);
        success = iset.add(key_type(1, 3), nullptr);
        REQUIRE(success == true);

        WHEN("adding overlap interval") {
            success = iset.add(key_type(2, 5), nullptr);
            REQUIRE(success == false);

            success = iset.add(key_type(2, 5), nullptr, true);
            REQUIRE(success == true);
            REQUIRE(iset.size() == 2);
            REQUIRE(iset.begin()->first == key_type(1, 6));
        }

        WHEN("removing overlaps") {
            iset.remove_overlaps(key_type(2, 9));
            REQUIRE(iset.empty() == true);
        }

        WHEN("subtract overlaps") {
            iset.subtract(key_type(2, 9));
            REQUIRE(iset.size() == 2);
            REQUIRE(iset.begin()->first == key_type(1, 2));
            REQUIRE((iset.begin() + 1)->first == key_type(9, 10));
        }
    }
}
