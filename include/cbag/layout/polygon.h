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

    inline polygon() : data() {}
    explicit inline polygon(std::size_t n) : data() { data.reserve(n); }
    explicit inline polygon(point_vector_t data) : data(std::move(data)) {}

    inline iterator_type begin() const { return data.begin(); }
    inline iterator_type end() const { return data.end(); }
    inline std::size_t size() const { return data.size(); }

    template <class iT> inline void set(iT input_begin, iT input_end) {
        data.clear();
        data.insert(data.end(), input_begin, input_end);
    }

  protected:
    point_vector_t data;
};

using polygon_set = std::vector<polygon>;

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::polygon> { using type = polygon_concept; };

} // namespace polygon
} // namespace boost
#endif
