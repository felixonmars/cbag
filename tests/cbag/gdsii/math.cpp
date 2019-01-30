#include <catch2/catch.hpp>

#include <cbag/gdsii/math.h>

TEST_CASE("No precision lost in gds <-> double conversion", "[gds]") {
    double value = GENERATE(values<double>({
        2.65732619e5,
        -2.65732619e5,
        13.7,
        1.6,
        1.0,
        0.0,
        0.1,
        1e-3,
        1.54e-5,
        1e-6,
        1e-9,
        -1.54e-5,
        -1e-6,
        -1e-9,
        90.0,
        180.0,
        270.0,
    }));

    uint64_t val_bits = *reinterpret_cast<uint64_t *>(&value);

    auto gds_code = cbag::gdsii::double_to_gds(value);

    // converts gds back to double
    auto val2 = cbag::gdsii::gds_to_double(gds_code);
    uint64_t val2_bits = *reinterpret_cast<uint64_t *>(&val2);

    REQUIRE(value == val2);
    // make sure really bit-by-bit matches
    REQUIRE(val_bits == val2_bits);
}

TEST_CASE("double -> gds conversion on specific cases", "[gds]") {
    std::pair<double, uint64_t> value = GENERATE(values<std::pair<double, uint64_t>>({
        {5e-4, 0x3e20c49ba5e353f8},
        {5e-10, 0x39225c17d04dad2a},
        {-5e-4, 0xbe20c49ba5e353f8},
        {-5e-10, 0xb9225c17d04dad2a},
        {0, 0x0000000000000000},
        {1.0, 0x4110000000000000},
        {10.0, 0x420a000000000000},
        {-3.0, 0xc130000000000000},
        {1e3, 0x433e800000000000},
        {1e5, 0x45186a0000000000},
        {-1e5, 0xc5186a0000000000},
    }));

    auto gds_code = cbag::gdsii::double_to_gds(value.first);
    REQUIRE(gds_code == value.second);
}
