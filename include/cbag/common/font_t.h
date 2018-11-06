
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
constexpr font_t fEuro = font_t::euroStyle;
constexpr font_t fGoth = font_t::gothic;
constexpr font_t fMath = font_t::math;
constexpr font_t fRoman = font_t::roman;
constexpr font_t fScript = font_t::script;
constexpr font_t fStick = font_t::stick;
constexpr font_t fFixed = font_t::fixed;
constexpr font_t fSwedish = font_t::swedish;
constexpr font_t fMilSpec = font_t::milSpec;

} // namespace cbag

#endif
