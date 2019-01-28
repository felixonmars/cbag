#include <cbag/common/box_t_util.h>
#include <cbag/layout/geo_index.h>
#include <cbag/layout/geo_iterator.h>
#include <cbag/layout/geometry.h>
#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

geo_index::geo_index() = default;

bool geo_index::empty() const { return index.empty(); }

box_t geo_index::get_bbox() const {
    box_t ans;
    auto box = index.bounds();
    set(ans, box.min_corner().get<0>(), box.min_corner().get<1>(), box.max_corner().get<0>(),
        box.max_corner().get<1>());
    return ans;
}

geo_iterator geo_index::begin_intersect(const box_t &r, offset_t spx, offset_t spy,
                                        const cbag::transformation &xform) const {

    return {r,
            spx,
            spy,
            index.qbegin(bgi::intersects(
                bg_box(bg_point(xl(r) - spx, yl(r) - spy), bg_point(xh(r) + spx, yh(r) + spy)))),
            index.qend(),
            xform};
}

void geo_index::insert(const geo_index *master, const cbag::transformation &xform) {
    if (!master->empty())
        index.insert(geo_object(geo_instance(master, xform), 0, 0));
}

} // namespace layout
} // namespace cbag
