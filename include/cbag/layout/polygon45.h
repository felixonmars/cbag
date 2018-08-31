/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_POLYGON45_H
#define CBAG_LAYOUT_POLYGON45_H

#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

class polygon45 : public polygon {
  public:
    using polygon::polygon;
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::polygon45> { using type = polygon_45_concept; };

// specialize traits to override winding
template <> struct polygon_traits<cbag::layout::polygon45> {
    using coordinate_type = cbag::layout::polygon45::coordinate_type;
    using iterator_type = cbag::layout::polygon45::iterator_type;
    using point_type = cbag::layout::polygon45::point_type;

    static inline iterator_type begin_points(const cbag::layout::polygon45 &t) { return t.begin(); }

    static inline iterator_type end_points(const cbag::layout::polygon45 &t) { return t.end(); }

    static inline std::size_t size(const cbag::layout::polygon45 &t) { return t.size(); }

    static inline winding_direction winding(const cbag::layout::polygon45 &t) {
        return t.winding();
    }
};

} // namespace polygon
} // namespace boost

#endif
