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

using pt_vector = std::vector<std::pair<coord_t, coord_t>>;

namespace traits {

template <typename T> struct pt_list {};
template <> struct pt_list<pt_vector> {
    using coordinate_type = coord_t;
    using point_type = std::pair<coord_t, coord_t>;

    static std::size_t size(const pt_vector &vec) { return vec.size(); }
    static coordinate_type x(const pt_vector &vec, std::size_t idx) { return vec[idx].first; }
    static coordinate_type y(const pt_vector &vec, std::size_t idx) { return vec[idx].second; }
    static void set_x(pt_vector &vec, std::size_t idx, coordinate_type val) {
        vec[idx].first = val;
    }
    static void set_y(pt_vector &vec, std::size_t idx, coordinate_type val) {
        vec[idx].second = val;
    }
    static auto begin(const pt_vector &vec) -> decltype(vec.begin()) { return vec.begin(); }
    static auto end(const pt_vector &vec) -> decltype(vec.end()) { return vec.end(); }
};
} // namespace traits

template <typename T> using IsPtVec = typename traits::pt_list<T>::coordinate_type;

} // namespace layout
} // namespace cbag

#endif
