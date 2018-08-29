/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_RECTANGLE_VIEW_H
#define CBAG_LAYOUT_RECTANGLE_VIEW_H

#include <cbag/common/typedefs.h>
#include <cbag/layout/polygon_sets.h>
#include <cbag/layout/rectangle.h>

namespace cbag {
namespace layout {

// -----------------------------------------------------------------------------
// rectangle declaration
// for rectangle, the first point is the lower left vertex, the second point
// is the upper right vertex.
// as the result, rectangle always have exactly 2 points in data.
// -----------------------------------------------------------------------------

class rectangle_view {
  public:
    using value_type = rectangle;
    using const_iterator = rectangle_set::const_iterator;
    using coordinate_type = coord_t;
    using operator_arg_type = rectangle_view;

  private:
    const rectangle_set *ptr;

  public:
    rectangle_view() = default;

    explicit rectangle_view(const rectangle_set &val) : ptr(&val) {}

    const_iterator begin() const { return ptr->begin(); }

    const_iterator end() const { return ptr->end(); }

    std::size_t size() const { return ptr->size(); }
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::rectangle_view> {
    using type = polygon_90_set_concept;
};

template <> struct polygon_90_set_traits<cbag::layout::rectangle_view> {
    using coordinate_type = cbag::layout::rectangle_view::coordinate_type;
    using iterator_type = cbag::layout::rectangle_view::const_iterator;
    using operator_arg_type = cbag::layout::rectangle_view::operator_arg_type;

    static inline iterator_type begin(const operator_arg_type &set) { return set.begin(); }
    static inline iterator_type end(const operator_arg_type &set) { return set.end(); }

    static inline orientation_2d orient(const operator_arg_type &set) { return HORIZONTAL; }
    static inline bool clean(const operator_arg_type &set) { return false; }
    static inline bool sorted(const operator_arg_type &set) { return false; }
};

} // namespace polygon
} // namespace boost

#endif