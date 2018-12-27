#include <string>
#include <tuple>

#include <catch2/catch.hpp>

#include "yaml-cpp/tuple.h"

TEST_CASE("Read tuples from yaml file", "[yaml]") {
    auto node = YAML::Load("[1, 2, -3]");
    auto expect1 = std::make_tuple(1, 2, -3);
    auto val1 = node.as<decltype(expect1)>();
    REQUIRE(val1 == expect1);

    node = YAML::Load("[1.5, 'foo']");
    auto expect2 = std::make_tuple(1.5, std::string("foo"));
    auto val2 = node.as<decltype(expect2)>();
    REQUIRE(val2 == expect2);
}

TEST_CASE("Write tuple to YAML file", "[yaml]") {
    auto val1 = std::make_tuple(1, 2, -3);
    auto node = YAML::convert<decltype(val1)>::encode(val1);
    REQUIRE(node.as<decltype(val1)>() == val1);

    auto val2 = std::make_tuple(1.5, std::string("foo"));
    node = YAML::convert<decltype(val2)>::encode(val2);
    REQUIRE(node.as<decltype(val2)>() == val2);
}
