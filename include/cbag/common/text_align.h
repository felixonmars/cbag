
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

} // namespace cbag

#endif
