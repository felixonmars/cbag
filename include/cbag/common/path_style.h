
#ifndef CBAG_COMMON_PATH_STYLE_H
#define CBAG_COMMON_PATH_STYLE_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using path_style = oa::oaPathStyleEnum;
constexpr path_style psTruncate = path_style::oacTruncatePathStyle;
constexpr path_style psExtend = path_style::oacExtendPathStyle;
constexpr path_style psRound = path_style::oacRoundPathStyle;
constexpr path_style psVariable = path_style::oacVariablePathStyle;

} // namespace cbag

#else

namespace cbag {

enum path_style : uint32_t { truncate, extend, round, variable };
constexpr path_style psTruncate = truncate;
constexpr path_style psExtend = extend;
constexpr path_style psRound = round;
constexpr path_style psVariable = variable;

} // namespace cbag

#endif
#endif
