#ifndef CBAG_LAYOUT_PT_VECTOR_H
#define CBAG_LAYOUT_PT_VECTOR_H

#include <utility>
#include <vector>

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>

namespace boost {
namespace polygon {
template <> struct geometry_concept<std::pair<cbag::coord_t, cbag::coord_t>> {
    using type = point_concept;
};

template <> struct point_traits<std::pair<cbag::coord_t, cbag::coord_t>> {
    using point_type = std::pair<cbag::coord_t, cbag::coord_t>;
    using coordinate_type = cbag::coord_t;

    static coordinate_type get(const point_type &point, orientation_2d orient) {
        return (orient.to_int() == static_cast<unsigned int>(orientation_2d_enum::HORIZONTAL))
                   ? point.first
                   : point.second;
    }
};
} // namespace polygon
} // namespace boost

namespace cbag {
namespace layout {

namespace traits {
template <typename T> struct pt_list {
    using coordinate_type = coord_t;

    static std::size_t size(const T &vec) { return vec.size(); }
    static coordinate_type x(const T &vec, std::size_t idx) { return vec[idx].first; }
    static coordinate_type y(const T &vec, std::size_t idx) { return vec[idx].second; }
    static void set_x(const T &vec, std::size_t idx, coordinate_type val) { vec[idx].first = val; }
    static void set_y(const T &vec, std::size_t idx, coordinate_type val) { vec[idx].second = val; }
};
} // namespace traits

using pt_vector = std::vector<std::pair<coord_t, coord_t>>;

} // namespace layout
} // namespace cbag

#endif
