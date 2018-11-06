
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
constexpr blockage_type blkRoute = blockage_type::routing;
constexpr blockage_type blkVia = blockage_type::via;
constexpr blockage_type blkPlacement = blockage_type::placement;
constexpr blockage_type blkWiring = blockage_type::wiring;
constexpr blockage_type blkFill = blockage_type::fill;
constexpr blockage_type blkSlot = blockage_type::slot;
constexpr blockage_type blkPin = blockage_type::pin;
constexpr blockage_type blkFeedThru = blockage_type::feed_thru;
constexpr blockage_type blkScreen = blockage_type::screen;

} // namespace cbag

#endif
