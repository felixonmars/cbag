/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_POLYGON45_VIEW_H
#define CBAG_LAYOUT_POLYGON45_VIEW_H

#include <cbag/layout/joined_ra_range.h>
#include <cbag/layout/polygon45.h>
#include <cbag/layout/polygon90_view.h>
#include <cbag/layout/rectangle.h>

namespace cbag {
namespace layout {

// -----------------------------------------------------------------------------
// rectangle declaration
// for rectangle, the first point is the lower left vertex, the second point
// is the upper right vertex.
// as the result, rectangle always have exactly 2 points in data.
// -----------------------------------------------------------------------------

class polygon45_view : public joined_ra_range<polygon45_set, polygon90_view> {
  public:
    using coordinate_type = coord_t;
    using operator_arg_type = polygon45_view;

    inline polygon45_view(const polygon45_set &val0, const polygon90_set &val1,
                          const rectangle_set &val2)
        : joined_ra_range<polygon45_set, polygon90_view>(val0, view_90), view_90(val1, val2) {}

  private:
    polygon90_view view_90;
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::polygon45_view> {
    using type = polygon_45_set_concept;
};

template <> struct polygon_set_traits<cbag::layout::polygon45_view> {
    using coordinate_type = typename cbag::layout::polygon45_view::coordinate_type;
    using iterator_type = typename cbag::layout::polygon45_view::const_iterator;
    using operator_arg_type = typename cbag::layout::polygon45_view::operator_arg_type;

    static inline iterator_type begin(const operator_arg_type &set) { return set.begin(); }
    static inline iterator_type end(const operator_arg_type &set) { return set.end(); }

    static inline bool clean(const operator_arg_type &set) { return false; }
    static inline bool sorted(const operator_arg_type &set) { return false; }
};

} // namespace polygon
} // namespace boost

#endif
