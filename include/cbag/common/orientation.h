
#ifndef CBAG_COMMON_ORIENTATION_H
#define CBAG_COMMON_ORIENTATION_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using orientation = oa::oaOrientEnum;
constexpr orientation oR0 = orientation::oacR0;
constexpr orientation oR90 = orientation::oacR90;
constexpr orientation oR180 = orientation::oacR180;
constexpr orientation oR270 = orientation::oacR270;
constexpr orientation oMY = orientation::oacMY;
constexpr orientation oMYR90 = orientation::oacMYR90;
constexpr orientation oMX = orientation::oacMX;
constexpr orientation oMXR90 = orientation::oacMXR90;

} // namespace cbag

#else

namespace cbag {

enum orientation : uint32_t { R0, R90, R180, R270, MY, MYR90, MX, MXR90 };
constexpr orientation oR0 = R0;
constexpr orientation oR90 = R90;
constexpr orientation oR180 = R180;
constexpr orientation oR270 = R270;
constexpr orientation oMY = MY;
constexpr orientation oMYR90 = MYR90;
constexpr orientation oMX = MX;
constexpr orientation oMXR90 = MXR90;

} // namespace cbag

#endif
#endif
