
#ifndef CBAG_GDSII_MATH_H
#define CBAG_GDSII_MATH_H

#include <cstdint>
#include <vector>

namespace cbag {
namespace gdsii {

constexpr auto gds_mantissa = 56;
constexpr auto gds_exp = 7;
constexpr auto gds_exp_max = ((1 << gds_exp) - 1) << 2;

uint64_t double_to_gds(double val);

double gds_to_double(uint64_t val);

std::vector<uint16_t> get_gds_time();

} // namespace gdsii
} // namespace cbag

#endif
