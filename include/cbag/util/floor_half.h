#ifndef CBAG_UTIL_FLOOR_HALF_H
#define CBAG_UTIL_FLOOR_HALF_H

#include <cbag/common/typedefs.h>

namespace cbag {
namespace util {

inline coord_t floor_half(coord_t a) { return a >> 1; }

} // namespace util
} // namespace cbag

#endif
