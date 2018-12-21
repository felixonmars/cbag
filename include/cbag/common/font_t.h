
#ifndef CBAG_COMMON_FONT_H
#define CBAG_COMMON_FONT_H

#include <cstdint>

namespace cbag {

enum class font_t : uint8_t {
    euroStyle = 0,
    gothic = 1,
    math = 2,
    roman = 3,
    script = 4,
    stick = 5,
    fixed = 6,
    swedish = 7,
    milSpec = 8
};

} // namespace cbag

#endif
