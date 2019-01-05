
#ifndef CBAG_ENUM_ORIENT_2D_H
#define CBAG_ENUM_ORIENT_2D_H

#include <boost/polygon/isotropy.hpp>

namespace cbag {

using orient_2d = boost::polygon::orientation_2d_enum;

using orient_2d_t = uint32_t;

inline orient_2d_t to_int(orient_2d val) { return static_cast<orient_2d_t>(val); }

inline orient_2d perpendicular(orient_2d val) { return static_cast<orient_2d>(1 - to_int(val)); }

} // namespace cbag

#endif
