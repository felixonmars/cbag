#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>
#include <cbag/layout/geo_instance.h>
#include <cbag/layout/geo_iterator.h>
#include <cbag/layout/geometry.h>

namespace cbag {
namespace layout {

geo_instance::geo_instance() = default;

geo_instance::geo_instance(const geometry *master, cbag::transformation xform)
    : master(master), xform(std::move(xform)) {}

bool geo_instance::empty() const { return master->index_empty(); }

box_t geo_instance::get_bbox() const {
    box_t ans = master->get_index_bbox();
    return transform(ans, xform);
}

geo_iterator geo_instance::begin_intersect(const box_t &r, offset_t spx, offset_t spy) const {
    auto spx_new = spx;
    auto spy_new = spy;
    auto test_r = get_transform(r, get_invert(xform));
    if (flips_xy(xform)) {
        spx_new = spy;
        spy_new = spx;
    }
    return master->begin_intersect(test_r, spx_new, spy_new, xform);
}

bool geo_instance::operator==(const geo_instance &rhs) const {
    return master == rhs.master && xform == rhs.xform;
}

} // namespace layout
} // namespace cbag
