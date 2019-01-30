
#ifndef CBAG_ENUM_ORIENTATION_H
#define CBAG_ENUM_ORIENTATION_H

#include <cstdint>

#include <boost/polygon/point_data.hpp>
#include <boost/polygon/transform.hpp>

namespace cbag {

using orientation = boost::polygon::axis_transformation::ATR;

using orient_t = uint32_t;

constexpr orientation oR0 = orientation::NULL_TRANSFORM;
constexpr orientation oR90 = orientation::ROTATE_RIGHT;
constexpr orientation oR180 = orientation::FLIP_XY;
constexpr orientation oR270 = orientation::ROTATE_LEFT;
constexpr orientation oMX = orientation::FLIP_Y;
constexpr orientation oMXR90 = orientation::SWAP_XY;
constexpr orientation oMY = orientation::FLIP_X;
constexpr orientation oMYR90 = orientation::FLIP_SWAP_XY;

inline std::string to_string(orientation orient) {
    switch (orient) {
    case oR0:
        return "R0";
    case oR90:
        return "R90";
    case oR180:
        return "R180";
    case oR270:
        return "R270";
    case oMX:
        return "MX";
    case oMXR90:
        return "MXR90";
    case oMY:
        return "MY";
    case oMYR90:
        return "MYR90";
    default:
        throw std::runtime_error("Unknown orientation code: " +
                                 std::to_string(static_cast<orient_t>(orient)));
    }
}

} // namespace cbag

#endif
