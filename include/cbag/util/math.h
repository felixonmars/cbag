#ifndef CBAG_UTIL_MATH_H
#define CBAG_UTIL_MATH_H

#include <cbag/common/typedefs.h>

namespace cbag {
namespace util {

// ceiling division
inline constexpr int ceil(int x, unsigned int y) { return x / y + (x % y > 0); }

// only works on arithmetic right shift architectures
inline constexpr int floor2(int a) { return a >> 1; }

} // namespace util
} // namespace cbag

#endif
