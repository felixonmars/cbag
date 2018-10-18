#include <algorithm>
#include <vector>

#include <catch2/catch.hpp>

#include <cbag/util/sorted_vector.h>

namespace cu = cbag::util;

SCENARIO("sorted_vector is empty", "[sorted_vector]") {
    cu::sorted_vector<int> v;
    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 0);
    REQUIRE(v.empty() == true);

    THEN("iterator behaves properly") {
        REQUIRE(v.begin() == v.end());
        REQUIRE(v.end() - v.begin() == 0);
    }

    THEN("at_front() and at_back() errors") {
        REQUIRE_THROWS_AS(v.at_front(), std::out_of_range);
        REQUIRE_THROWS_AS(v.at_back(), std::out_of_range);
    }

    THEN("lower_bound works") { REQUIRE(v.lower_bound(0) == v.end()); }

    THEN("equal_range works") {
        auto iter_range = v.equal_range(0);
        REQUIRE(iter_range.first == v.end());
        REQUIRE(iter_range.second == v.end());
    }
}

SCENARIO("sorted_vector has some values", "[sorted_vector]") {
    std::vector<int> data{6, 2, 4};
    std::vector<int> expected(data);
    std::sort(expected.begin(), expected.end());

    WHEN("initializer list constructor") {
        cu::sorted_vector<int> v{data[0], data[1], data[2]};

        THEN("vector has right size and is sorted") {
            REQUIRE(v.size() == expected.size());
            REQUIRE(v.capacity() >= expected.size());
            REQUIRE(v == expected);
        }
    }

    WHEN("std vector constructor") {
        cu::sorted_vector<int> v(data);

        THEN("vector has right size and is sorted") {
            REQUIRE(v.size() == expected.size());
            REQUIRE(v.capacity() >= expected.size());
            REQUIRE(v == expected);
        }

        THEN("iterator behaves properly") {
            REQUIRE(*v.begin() == expected[0]);
            REQUIRE(v.begin() != v.end());
            REQUIRE(v.end() - v.begin() == v.size());
        }

        THEN("at_front() and at_back() works") {
            REQUIRE(v.at_front() == expected.front());
            REQUIRE(v.at_back() == expected.back());
        }

        THEN("lower_bound works") {
            REQUIRE(v.lower_bound(expected[0] - 1) - v.begin() == 0);
            REQUIRE(v.lower_bound(expected[0]) - v.begin() == 0);
            REQUIRE(v.lower_bound(expected[0] + 1) - v.begin() == 1);
            REQUIRE(v.lower_bound(expected[1]) - v.begin() == 1);
            REQUIRE(v.lower_bound(expected[2]) - v.begin() == 2);
            REQUIRE(v.lower_bound(expected[2] + 1) == v.end());
        }

        THEN("equal_range works") {
            auto iter_range = v.equal_range(expected[0] - 1);
            REQUIRE(iter_range.first - v.begin() == 0);
            REQUIRE(iter_range.second - v.begin() == 0);
            iter_range = v.equal_range(expected[0]);
            REQUIRE(iter_range.first - v.begin() == 0);
            REQUIRE(iter_range.second - v.begin() == 1);
            iter_range = v.equal_range(expected[0] + 1);
            REQUIRE(iter_range.first - v.begin() == 1);
            REQUIRE(iter_range.second - v.begin() == 1);
            iter_range = v.equal_range(expected[1]);
            REQUIRE(iter_range.first - v.begin() == 1);
            REQUIRE(iter_range.second - v.begin() == 2);
            iter_range = v.equal_range(expected[2]);
            REQUIRE(iter_range.first - v.begin() == 2);
            REQUIRE(iter_range.second == v.end());
            iter_range = v.equal_range(expected[2] + 1);
            REQUIRE(iter_range.first == v.end());
            REQUIRE(iter_range.second == v.end());
        }
    }
}

SCENARIO("sorted_vector with duplicate values", "[sorted_vector]") {
    std::vector<int> data{6, 2, 4, 4, 4};
    std::vector<int> expected(data);
    std::sort(expected.begin(), expected.end());
    cu::sorted_vector<int> v(data);

    THEN("vector has right size and is sorted") {
        REQUIRE(v.size() == expected.size());
        REQUIRE(v.capacity() >= expected.size());
        REQUIRE(v == expected);
    }

    THEN("lower_bound works") {
        REQUIRE(v.lower_bound(expected[0] - 1) - v.begin() == 0);
        REQUIRE(v.lower_bound(expected[0]) - v.begin() == 0);
        REQUIRE(v.lower_bound(expected[0] + 1) - v.begin() == 1);
        REQUIRE(v.lower_bound(expected[1]) - v.begin() == 1);
        REQUIRE(v.lower_bound(expected.back()) - v.begin() == expected.size() - 1);
        REQUIRE(v.lower_bound(expected.back() + 1) == v.end());
    }

    THEN("equal_range works") {
        auto iter_range = v.equal_range(expected[0] - 1);
        REQUIRE(iter_range.first - v.begin() == 0);
        REQUIRE(iter_range.second - v.begin() == 0);
        iter_range = v.equal_range(expected[0]);
        REQUIRE(iter_range.first - v.begin() == 0);
        REQUIRE(iter_range.second - v.begin() == 1);
        iter_range = v.equal_range(expected[0] + 1);
        REQUIRE(iter_range.first - v.begin() == 1);
        REQUIRE(iter_range.second - v.begin() == 1);
        iter_range = v.equal_range(expected[1]);
        REQUIRE(iter_range.first - v.begin() == 1);
        REQUIRE(iter_range.second - v.begin() == 4);
        iter_range = v.equal_range(expected.back());
        REQUIRE(iter_range.first - v.begin() == expected.size() - 1);
        REQUIRE(iter_range.second == v.end());
        iter_range = v.equal_range(expected.back() + 1);
        REQUIRE(iter_range.first == v.end());
        REQUIRE(iter_range.second == v.end());
    }
}

SCENARIO("sorted_vector insertion", "[sorted_vector]") {
    cu::sorted_vector<int> v;
    auto ans = v.insert_unique(3);
    REQUIRE(v.size() == 1);
    REQUIRE(v.at_front() == 3);
    REQUIRE(v.at_back() == 3);
    REQUIRE(ans.first - v.begin() == 0);
    REQUIRE(ans.second == true);

    ans = v.emplace_unique(1);
    REQUIRE(v.size() == 2);
    REQUIRE(v.at_front() == 1);
    REQUIRE(v.at_back() == 3);
    REQUIRE(ans.first - v.begin() == 0);
    REQUIRE(ans.second == true);

    ans = v.insert_unique(2);
    std::vector<int> expected{1, 2, 3};
    REQUIRE(v == expected);
    REQUIRE(ans.first - v.begin() == 1);
    REQUIRE(ans.second == true);

    THEN("insert/emplace duplicate values fail") {
        ans = v.insert_unique(2);
        REQUIRE(v == expected);
        REQUIRE(ans.first - v.begin() == 1);
        REQUIRE(ans.second == false);

        ans = v.emplace_unique(2);
        REQUIRE(v == expected);
        REQUIRE(ans.first - v.begin() == 1);
        REQUIRE(ans.second == false);
    }
}

SCENARIO("sorted_vector deletion", "[sorted_vector]") {
    std::vector<int> expected{2, 4, 6, 8, 10};
    cu::sorted_vector<int> v(expected);

    WHEN("erasing one element") {
        v.erase(v.begin() + 1);
        expected.erase(expected.begin() + 1);
        REQUIRE(v == expected);
    }

    WHEN("erasing zero element") {
        v.erase(v.begin() + 1, v.begin() + 1);
        expected.erase(expected.begin() + 1, expected.begin() + 1);
        REQUIRE(v == expected);
    }

    WHEN("erasing multiple element") {
        v.erase(v.begin() + 1, v.begin() + 4);
        expected.erase(expected.begin() + 1, expected.begin() + 4);
        REQUIRE(v == expected);
    }
}
