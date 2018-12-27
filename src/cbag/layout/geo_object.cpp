#include <boost/polygon/polygon.hpp>

#include <cbag/common/box_t_adapt.h>
#include <cbag/common/box_t_util.h>
#include <cbag/layout/geo_object.h>
#include <cbag/util/overload.h>

namespace bp = boost::polygon;

namespace cbag {
namespace layout {

bool geo_object::operator==(const geo_object &v) const {
    return val == v.val && spx == v.spx && spy == v.spy;
}

const geo_instance *geo_object::get_instance() const { return std::get_if<geo_instance>(&val); }

bg_box get_bnd_box(const geo_object::value_type &val, offset_t spx, offset_t spy) {
    box_t box;
    std::visit(overload{[&box](const box_t &v) { box = v; },
                        [&box](const polygon90 &v) {
                            bool success = bp::extents(box, v);
                            if (!success)
                                box = box_t::get_invalid_box();
                        },
                        [&box](const polygon45 &v) {
                            bool success = bp::extents(box, v);
                            if (!success)
                                box = box_t::get_invalid_box();
                        },
                        [&box](const polygon &v) {
                            bool success = bp::extents(box, v);
                            if (!success)
                                box = box_t::get_invalid_box();
                        },
                        [&box](const geo_instance &v) { box = v.get_bbox(); }},
               val);

    if (!is_valid(box))
        throw std::invalid_argument("Cannot add geometry object with invalid bound box.");
    return bg_box(bg_point(xl(box) - spx, yl(box) - spy), bg_point(xh(box) + spx, yh(box) + spy));
}

} // namespace layout
} // namespace cbag
