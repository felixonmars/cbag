
#ifndef CBAG_GDSII_MATH_H
#define CBAG_GDSII_MATH_H

#include <cstdint>
#include <vector>

namespace cbag {
namespace gdsii {

uint64_t convert_double(double val);

std::vector<uint16_t> get_gds_time();

} // namespace gdsii
} // namespace cbag

#endif
