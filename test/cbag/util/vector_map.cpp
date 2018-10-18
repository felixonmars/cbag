#include <catch2/catch.hpp>

#include <cbag/util/vector_map.h>

namespace cu = cbag::util;

SCENARIO("vector_map is empty", "[vector_map]") {
    cu::vector_map<int, int> m;
    REQUIRE(m.size() == 0);
    REQUIRE(m.empty() == true);

    THEN("iterator behaves properly") {
        REQUIRE(m.begin() == m.end());
        REQUIRE(m.end() - m.begin() == 0);
    }

    THEN("at_front() and at_back() errors") {
        REQUIRE_THROWS_AS(m.at_front(), std::out_of_range);
        REQUIRE_THROWS_AS(m.at_back(), std::out_of_range);
    }

    THEN("find/find_exact works") {
        REQUIRE(m.find(0) == m.end());
        REQUIRE(m.find_exact(0) == m.end());
    }

    THEN("equal_size/equal_range works") {
        REQUIRE(m.equal_size(0) == 0);
        auto iter_range = m.equal_range(0);
        REQUIRE(iter_range.first == m.end());
        REQUIRE(iter_range.second == m.end());
    }

    WHEN("adding values") {
        auto ans = m.emplace(4, 4);
        REQUIRE(ans.first == m.begin());
        REQUIRE(ans.second == true);
        REQUIRE(m.size() == 1);
        REQUIRE(m.at_front() == std::make_pair(4, 4));
        REQUIRE(m.at_back() == std::make_pair(4, 4));

        ans = m.emplace(3, 3);
        REQUIRE(ans.first == m.begin());
        REQUIRE(ans.second == true);
        REQUIRE(m.size() == 2);
        REQUIRE(m.at_front() == std::make_pair(3, 3));
        REQUIRE(m.at_back() == std::make_pair(4, 4));
    }
}
