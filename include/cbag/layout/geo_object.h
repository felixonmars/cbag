#ifndef CBAG_LAYOUT_GEO_OBJECT_H
#define CBAG_LAYOUT_GEO_OBJECT_H

#include <variant>

#include <boost/geometry.hpp>
#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>
#include <cbag/layout/geo_instance.h>
#include <cbag/layout/polygon.h>
#include <cbag/layout/polygon45_fwd.h>
#include <cbag/layout/polygon90_fwd.h>
#include <cbag/layout/rectangle.h>

namespace bg = boost::geometry;

namespace cbag {
namespace layout {

using bg_point = bg::model::point<coord_t, 2, bg::cs::cartesian>;
using bg_box = bg::model::box<bg_point>;

class geo_object {
  public:
    using value_type = std::variant<rectangle, polygon90, polygon45, polygon, geo_instance>;
    using box_type = bg_box;

  private:
    value_type val;
    offset_t spx = 0;
    offset_t spy = 0;

  public:
    box_type bnd_box;

    template <typename T>
    geo_object(T v, offset_t spx, offset_t spy)
        : val(std::move(v)), spx(spx), spy(spy), bnd_box(geo_object::get_bnd_box(val, spx, spy)) {}

    bool operator==(const geo_object &v) const;

    static box_type get_bnd_box(const value_type &val, offset_t spx, offset_t spy);
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace geometry {
namespace index {

template <> struct indexable<cbag::layout::geo_object> {
    using result_type = const cbag::layout::geo_object::box_type &;

    result_type operator()(const cbag::layout::geo_object &v) const { return v.bnd_box; }
};

} // namespace index
} // namespace geometry
} // namespace boost

#endif
