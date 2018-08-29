/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_POLYGON45_VIEW_H
#define CBAG_LAYOUT_POLYGON45_VIEW_H

#include <cbag/layout/joined_ra_range.h>
#include <cbag/layout/polygon90_view.h>
#include <cbag/layout/polygon_sets.h>

namespace cbag {
namespace layout {

class polygon45_view : public joined_ra_range<polygon45_set, polygon90_view> {
  public:
    using coordinate_type = coord_t;
    using operator_arg_type = polygon45_view;

  private:
    polygon90_view view_90;

  public:
    polygon45_view(const polygon45_set &val0, const polygon90_set &val1, const rectangle_set &val2);
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::polygon45_view> {
    using type = polygon_45_set_concept;
};

template <> struct polygon_45_set_traits<cbag::layout::polygon45_view> {
    using coordinate_type = cbag::layout::polygon45_view::coordinate_type;
    using iterator_type = cbag::layout::polygon45_view::const_iterator;
    using operator_arg_type = cbag::layout::polygon45_view::operator_arg_type;

    static inline iterator_type begin(const operator_arg_type &set) { return set.begin(); }
    static inline iterator_type end(const operator_arg_type &set) { return set.end(); }

    static inline bool clean(const operator_arg_type &set) { return false; }
    static inline bool sorted(const operator_arg_type &set) { return false; }
};

} // namespace polygon
} // namespace boost

#endif
