#ifndef CBAG_LAYOUT_PT_VECTOR_H
#define CBAG_LAYOUT_PT_VECTOR_H

#include <vector>

#include <cbag/common/typedefs.h>
#include <cbag/layout/pt_list.h>
#include <cbag/layout/pt_traits.h>

namespace cbag {
namespace layout {

using pt_vector = std::vector<point>;

namespace traits {

template <> struct pt_list<pt_vector> {
    using coordinate_type = coord_t;

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

} // namespace layout
} // namespace cbag

#endif
