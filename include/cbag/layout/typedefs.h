/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_TYPEDEFS_H
#define CBAG_LAYOUT_TYPEDEFS_H

#include <variant>
#include <vector>

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>

namespace bp = boost::polygon;

namespace cbag {
namespace layout {

using point_t = bp::point_data<coord_t>;
using point_vector_t = std::vector<point_t>;
using winding_dir = bp::winding_direction;

// custom unsigned char literal definition
inline constexpr unsigned char operator"" _uc(unsigned long long arg) noexcept {
    return static_cast<unsigned char>(arg);
}

} // namespace layout
} // namespace cbag

#endif
