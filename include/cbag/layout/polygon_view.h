/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_POLYGON_VIEW_H
#define CBAG_LAYOUT_POLYGON_VIEW_H

#include <cbag/common/typedefs.h>
#include <cbag/layout/joined_ra_range.h>
#include <cbag/layout/polygon90_view.h>
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

using polygon_view2 = joined_ra_range<polygon_set, polygon45_set>;

class polygon_view : public joined_ra_range<polygon_view2, polygon90_view> {
  public:
    using coordinate_type = coord_t;
    using operator_arg_type = polygon_view;

  private:
    polygon_view2 view_no_90;
    polygon90_view view_90;

  public:
    polygon_view(const polygon_set &val0, const polygon45_set &val1, const polygon90_set &val2,
                 const rectangle_set &val3);
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::polygon_view> {
    using type = polygon_set_concept;
};

template <> struct polygon_set_traits<cbag::layout::polygon_view> {
    using coordinate_type = cbag::layout::polygon_view::coordinate_type;
    using iterator_type = cbag::layout::polygon_view::const_iterator;
    using operator_arg_type = cbag::layout::polygon_view::operator_arg_type;

    static inline iterator_type begin(const operator_arg_type &set) { return set.begin(); }
    static inline iterator_type end(const operator_arg_type &set) { return set.end(); }

    static inline bool clean(const operator_arg_type &set) { return false; }
    static inline bool sorted(const operator_arg_type &set) { return false; }
};

} // namespace polygon
} // namespace boost

#endif
