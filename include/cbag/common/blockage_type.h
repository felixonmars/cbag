
#ifndef CBAG_COMMON_BLOCKAGE_TYPE_H
#define CBAG_COMMON_BLOCKAGE_TYPE_H

#include <cstdint>

namespace cbag {

enum class blockage_type : uint32_t {
    routing = 0,
    via = 1,
    placement = 2,
    wiring = 3,
    fill = 4,
    slot = 5,
    pin = 6,
    feed_thru = 7,
    screen = 8,
};

} // namespace cbag

#endif
