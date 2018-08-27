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
    inline polygon45() : polygon() {}
    explicit inline polygon45(std::size_t n) : polygon(n) {}
    explicit inline polygon45(point_vector_t data) : polygon(std::move(data)) {}
};

using polygon45_set = std::vector<polygon45>;

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::polygon45> { using type = polygon_45_concept; };

} // namespace polygon
} // namespace boost

#endif
