#include <cbag/layout/cellview.h>
#include <cbag/layout/geo_object.h>
#include <cbag/util/overload.h>

namespace cbag {
namespace layout {

bool geo_object::operator==(const geo_object &v) const {
    return val == v.val && spx == v.spx && spy == v.spy;
}

geo_object::box_type geo_object::get_bnd_box(const value_type &val, offset_t spx, offset_t spy,
                                             const char *lay, const char *purp) {
    rectangle box;
    std::visit(overload{[&](const rectangle &v) { box = v; },
                        [&](const cellview *v) { box = v->get_bbox(lay, purp); },
                        [&](const polygon90 &v) { bp::extents(box, v); },
                        [&](const polygon45 &v) { bp::extents(box, v); },
                        [&](const polygon &v) { bp::extents(box, v); }},
               val);

    if (!box.is_valid())
        throw std::invalid_argument("Cannot add invalid geometry object.");
    return bg_box(bg_point(box.xl - spx, box.yl - spy), bg_point(box.xh + spx, box.yh + spy));
}

} // namespace layout
} // namespace cbag
