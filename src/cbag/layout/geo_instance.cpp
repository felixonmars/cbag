#include <cbag/common/box_t.h>
#include <cbag/layout/geo_instance.h>
#include <cbag/layout/geo_iterator.h>
#include <cbag/layout/geometry.h>

namespace cbag {
namespace layout {

geo_instance::geo_instance() = default;

geo_instance::geo_instance(const geometry *master, cbag::transformation xform)
    : master(master), xform(std::move(xform)) {}

bool geo_instance::empty() const { return master->index_empty(); }

box_t &geo_instance::get_bbox(box_t &r) const {
    master->get_index_bbox(r);
    return r.transform(xform);
}

geo_iterator geo_instance::begin_intersect(const box_t &r, offset_t spx, offset_t spy) const {
    box_t test_r = r.get_transform(xform.get_inverse());
    if (xform.flip_xy()) {
        return master->begin_intersect(test_r, spy, spx, xform);
    }
    return master->begin_intersect(test_r, spx, spy, xform);
}

geo_iterator geo_instance::end_intersect() const { return {}; }

bool geo_instance::operator==(const geo_instance &rhs) const {
    return master == rhs.master && xform == rhs.xform;
}

} // namespace layout
} // namespace cbag
