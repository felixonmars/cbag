
#ifndef CBAG_COMMON_PATH_STYLE_H
#define CBAG_COMMON_PATH_STYLE_H

#include <cstdint>

namespace cbag {

enum class path_style : uint32_t { truncate = 0, extend = 1, round = 2, variable = 3 };
constexpr path_style psTruncate = path_style::truncate;
constexpr path_style psExtend = path_style::extend;
constexpr path_style psRound = path_style::round;
constexpr path_style psVariable = path_style::variable;

} // namespace cbag

#endif
