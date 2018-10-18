#include <algorithm>
#include <vector>

#include <catch2/catch.hpp>

#include <cbag/util/vector_map.h>

namespace cu = cbag::util;

SCENARIO("vector_map is empty", "[vector_map]") {
    cu::vector_map<int, int> v;
    REQUIRE(v.size() == 0);
    REQUIRE(v.empty() == true);

    THEN("iterator behaves properly") {
        REQUIRE(v.begin() == v.end());
        REQUIRE(v.end() - v.begin() == 0);
    }

    THEN("at_front() and at_back() errors") {
        REQUIRE_THROWS_AS(v.at_front(), std::out_of_range);
        REQUIRE_THROWS_AS(v.at_back(), std::out_of_range);
    }

    THEN("find/find_exact works") {
        REQUIRE(v.find(0) == v.end());
        REQUIRE(v.find_exact(0) == v.end());
    }

    THEN("equal_size/equal_range works") {
        REQUIRE(v.equal_size(0) == 0);
        auto iter_range = v.equal_range(0);
        REQUIRE(iter_range.first == v.end());
        REQUIRE(iter_range.second == v.end());
    }

    WHEN("adding values") {

    }
}
