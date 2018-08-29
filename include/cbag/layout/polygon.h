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
    using area_type =
        typename bp::area_type_by_domain<typename bp::geometry_domain<bp::polygon_concept>,
                                         coordinate_type>::type;

  protected:
    point_vector_t data;
    mutable winding_dir wdir = winding_dir::unknown_winding;

  public:
    polygon();

    explicit polygon(std::size_t n);

    explicit polygon(point_vector_t data, winding_dir wdir = winding_dir::unknown_winding);

    iterator_type begin() const { return data.begin(); }
    iterator_type end() const { return data.end(); }
    std::size_t size() const { return data.size(); }
    winding_dir winding() const;

    template <class iT> inline void set(iT input_begin, iT input_end) {
        data.clear();
        data.insert(data.end(), input_begin, input_end);
        wdir = winding_dir::unknown_winding;
    }
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::polygon> { using type = polygon_concept; };

// specialize traits to override winding
template <> struct polygon_traits<cbag::layout::polygon> {
    using coordinate_type = cbag::layout::polygon::coordinate_type;
    using iterator_type = cbag::layout::polygon::iterator_type;
    using point_type = cbag::layout::polygon::point_type;

    static inline iterator_type begin_points(const cbag::layout::polygon &t) { return t.begin(); }

    static inline iterator_type end_points(const cbag::layout::polygon &t) { return t.end(); }

    static inline std::size_t size(const cbag::layout::polygon &t) { return t.size(); }

    static inline winding_direction winding(const cbag::layout::polygon &t) { return t.winding(); }
};

} // namespace polygon
} // namespace boost
#endif
