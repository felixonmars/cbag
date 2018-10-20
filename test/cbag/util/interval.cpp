#include <catch2/catch.hpp>

#include <cbag/util/interval.h>

namespace cu = cbag::util;

using coord_t = cbag::offset_t;

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
        bool success = iset.emplace(false, false, 8, 10);
        REQUIRE(success == true);
        success = iset.emplace(false, false, 4, 6);
        REQUIRE(success == true);
        success = iset.emplace(false, false, 1, 3);
        REQUIRE(success == true);

        WHEN("adding overlap interval") {
            success = iset.emplace(false, false, 2, 5);
            REQUIRE(success == false);

            success = iset.emplace(true, false, 2, 5);
            REQUIRE(success == true);
            REQUIRE(iset.size() == 2);
            REQUIRE(*iset.begin() == cu::intv_type(1, 6));
        }

        WHEN("removing overlaps") {
            iset.remove_overlaps(cu::intv_type(2, 9));
            REQUIRE(iset.empty() == true);
        }

        WHEN("subtract overlaps") {
            iset.subtract(cu::intv_type(2, 9));
            REQUIRE(iset.size() == 2);
            REQUIRE(*iset.begin() == cu::intv_type(1, 2));
            REQUIRE(*(iset.begin() + 1) == cu::intv_type(9, 10));
        }
    }
}
