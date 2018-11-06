
#ifndef CBAG_COMMON_TYPEDEFS_H
#define CBAG_COMMON_TYPEDEFS_H

#include <cstdint>
#include <utility>

namespace cbag {

using coord_t = int32_t;
using lay_t = uint32_t;
using purp_t = uint32_t;
using dist_t = uint32_t;
using offset_t = int32_t;

using layer_t = std::pair<lay_t, purp_t>;

// custom unsigned char literal definition
inline constexpr unsigned char operator"" _uc(unsigned long long arg) noexcept {
    return static_cast<unsigned char>(arg);
}
} // namespace cbag

#endif
