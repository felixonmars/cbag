/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_POLYGON_H
#define CBAG_LAYOUT_POLYGON_H

#include <cbag/layout/typedefs.h>

namespace cbag {
namespace layout {

class polygon {
  public:
    using coordinate_type = coord_t;
    using iterator_type = point_vector_t::const_iterator;
    using point_type = point_t;

  protected:
    point_vector_t data;

  public:
    polygon();

    explicit polygon(std::size_t n);

    explicit polygon(point_vector_t data);

    iterator_type begin() const { return data.begin(); }
    iterator_type end() const { return data.end(); }
    std::size_t size() const { return data.size(); }

    template <class iT> inline void set(iT input_begin, iT input_end) {
        data.clear();
        data.insert(data.end(), input_begin, input_end);
    }
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::polygon> { using type = polygon_concept; };

} // namespace polygon
} // namespace boost
#endif
