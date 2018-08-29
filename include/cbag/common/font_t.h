
#ifndef CBAG_COMMON_FONT_H
#define CBAG_COMMON_FONT_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using font_t = oa::oaFontEnum;
constexpr font_t fEuro = font_t::oacEuroStyleFont;
constexpr font_t fGoth = font_t::oacGothicFont;
constexpr font_t fMath = font_t::oacMathFont;
constexpr font_t fRoman = font_t::oacRomanFont;
constexpr font_t fScript = font_t::oacScriptFont;
constexpr font_t fStick = font_t::oacStickFont;
constexpr font_t fFixed = font_t::oacFixedFont;
constexpr font_t fSwedish = font_t::oacSwedishFont;
constexpr font_t fMilSpec = font_t::oacMilSpecFont;

} // namespace cbag

#else

namespace cbag {

enum font_t : uint32_t { euroStyle, gothic, math, roman, script, stick, fixed, swedish, milSpec };
constexpr font_t fEuro = euroStyle;
constexpr font_t fGoth = gothic;
constexpr font_t fMath = math;
constexpr font_t fRoman = roman;
constexpr font_t fScript = script;
constexpr font_t fStick = stick;
constexpr font_t fFixed = fixed;
constexpr font_t fSwedish = swedish;
constexpr font_t fMilSpec = milSpec;

} // namespace cbag

#endif
#endif
