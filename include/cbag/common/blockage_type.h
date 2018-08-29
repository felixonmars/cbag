
#ifndef CBAG_COMMON_BLOCKAGE_TYPE_H
#define CBAG_COMMON_BLOCKAGE_TYPE_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using blockage_type = oa::oaBlockageTypeEnum;
constexpr blockage_type blkRoute = blockage_type::oacRoutingBlockageType;
constexpr blockage_type blkVia = blockage_type::oacViaBlockageType;
constexpr blockage_type blkPlacement = blockage_type::oacPlacementBlockageType;
constexpr blockage_type blkWiring = blockage_type::oacWiringBlockageType;
constexpr blockage_type blkFill = blockage_type::oacFillBlockageType;
constexpr blockage_type blkSlot = blockage_type::oacSlotBlockageType;
constexpr blockage_type blkPin = blockage_type::oacPinBlockageType;
constexpr blockage_type blkFeedThru = blockage_type::oacFeedthruBlockageType;
constexpr blockage_type blkScreen = blockage_type::oacScreenBlockageType;

} // namespace cbag

#else

#include <cstdint>

namespace cbag {

enum blockage_type : uint32_t {
    routing,
    via,
    placement,
    wiring,
    fill,
    slot,
    pin,
    feed_thru,
    screen
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
#endif
