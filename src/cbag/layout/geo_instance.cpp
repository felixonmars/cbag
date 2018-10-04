#include <cbag/layout/geo_instance.h>
#include <cbag/layout/geometry.h>
#include <cbag/layout/rectangle.h>

namespace cbag {
namespace layout {

geo_instance::geo_instance() = default;

geo_instance::geo_instance(const geometry *master, transformation xform)
    : master(master), xform(std::move(xform)) {}

bool geo_instance::empty() const { return master->index_empty(); }

rectangle &geo_instance::get_bbox(rectangle &r) const {
    master->get_index_bbox(r);
    return r.transform(xform);
}

bool geo_instance::operator==(const geo_instance &rhs) const {
    return master == rhs.master && xform == rhs.xform;
}

} // namespace layout
} // namespace cbag
