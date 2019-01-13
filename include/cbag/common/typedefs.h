
#ifndef CBAG_COMMON_TYPEDEFS_H
#define CBAG_COMMON_TYPEDEFS_H

#include <cstdint>
#include <tuple>

namespace cbag {

using coord_t = int32_t;
using lay_t = uint32_t;
using purp_t = uint32_t;
using dist_t = uint32_t;
using offset_t = int32_t;
using enum_t = uint_fast8_t;
using senum_t = int_fast8_t;
using cnt_t = uint_fast32_t;
using htr_t = int_fast32_t;
using level_t = int_fast32_t;
using temp_t = int_fast32_t;
using scnt_t = int_fast32_t;
using em_specs_t = std::tuple<double, double, double>;

} // namespace cbag

#endif
