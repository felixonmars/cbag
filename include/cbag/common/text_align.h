
#ifndef CBAG_COMMON_TEXT_ALIGN_H
#define CBAG_COMMON_TEXT_ALIGN_H

#include <cstdint>

namespace cbag {

enum class text_align : uint8_t {
    upperLeft = 0,
    centerLeft = 1,
    lowerLeft = 2,
    upperCenter = 3,
    centerCenter = 4,
    lowerCenter = 5,
    upperRight = 6,
    centerRight = 7,
    lowerRight = 8
};
constexpr text_align taUL = text_align::upperLeft;
constexpr text_align taCL = text_align::centerLeft;
constexpr text_align taLL = text_align::lowerLeft;
constexpr text_align taUC = text_align::upperCenter;
constexpr text_align taCC = text_align::centerCenter;
constexpr text_align taLC = text_align::lowerCenter;
constexpr text_align taUR = text_align::upperRight;
constexpr text_align taCR = text_align::centerRight;
constexpr text_align taLR = text_align::lowerRight;

} // namespace cbag

#endif
