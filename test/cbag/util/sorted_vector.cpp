#include <algorithm>
#include <vector>

#include <catch2/catch.hpp>

#include <cbag/util/sorted_vector.h>

namespace cu = cbag::util;

SCENARIO("sorted_vector construction with given values", "[sorted_vector]") {
    GIVEN("some data") {
        std::vector<int> data{3, 1, 2};
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
        }
    }
}
