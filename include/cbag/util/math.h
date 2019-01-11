#ifndef CBAG_UTIL_MATH_H
#define CBAG_UTIL_MATH_H

#include <climits>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace util {

// ceiling division
inline constexpr int ceil(int x, unsigned int y) { return x / y + (x % y > 0); }

// only works on arithmetic right shift architectures
inline constexpr int floor2(int a) { return a >> 1; }

// only works on arithmetic right shift architectures, n must be positive
inline constexpr int pos_mod(int i, int n) {
    constexpr int shift = CHAR_BIT * sizeof(int) - 1;
    int m = i % n;
    return m + ((m >> shift) & n);
}

} // namespace util
} // namespace cbag

#endif
