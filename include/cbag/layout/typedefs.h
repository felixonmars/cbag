/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_TYPEDEFS_H
#define CBAG_LAYOUT_TYPEDEFS_H

#include <utility>
#include <variant>
#include <vector>

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>

namespace bp = boost::polygon;

namespace cbag {
namespace layout {

using point_t = bp::point_data<coord_t>;
using pt_vector = std::vector<point_t>;
using polygon90 = bp::polygon_90_data<coord_t>;
using polygon45 = bp::polygon_45_data<coord_t>;
using polygon = bp::polygon_data<coord_t>;
using polygon90_set = bp::polygon_90_set_data<coord_t>;
using polygon45_set = bp::polygon_45_set_data<coord_t>;
using polygon_set = bp::polygon_set_data<coord_t>;

using layer_t = std::pair<lay_t, purp_t>;

// custom unsigned char literal definition
inline constexpr unsigned char operator"" _uc(unsigned long long arg) noexcept {
    return static_cast<unsigned char>(arg);
}

} // namespace layout
} // namespace cbag

#endif
