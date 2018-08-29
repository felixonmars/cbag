
#ifndef CBAG_COMMON_TEXT_ALIGN_H
#define CBAG_COMMON_TEXT_ALIGN_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using text_align = oa::oaTextAlignEnum;
constexpr text_align taUL = text_align::oacUpperLeftTextAlign;
constexpr text_align taCL = text_align::oacCenterLeftTextAlign;
constexpr text_align taLL = text_align::oacLowerLeftTextAlign;
constexpr text_align taUC = text_align::oacUpperCenterTextAlign;
constexpr text_align taCC = text_align::oacCenterCenterTextAlign;
constexpr text_align taLC = text_align::oacLowerCenterTextAlign;
constexpr text_align taUR = text_align::oacUpperRightTextAlign;
constexpr text_align taCR = text_align::oacCenterRightTextAlign;
constexpr text_align taLR = text_align::oacLowerRightTextAlign;

} // namespace cbag

#else

namespace cbag {

enum text_align : uint32_t {
    upperLeft,
    centerLeft,
    lowerLeft,
    upperCenter,
    centerCenter,
    lowerCenter,
    upperRight,
    centerRight,
    lowerRight
};
constexpr text_align taUL = upperLeft;
constexpr text_align taCL = centerLeft;
constexpr text_align taLL = lowerLeft;
constexpr text_align taUC = upperCenter;
constexpr text_align taCC = centerCenter;
constexpr text_align taLC = lowerCenter;
constexpr text_align taUR = upperRight;
constexpr text_align taCR = centerRight;
constexpr text_align taLR = lowerRight;

} // namespace cbag

#endif
#endif
