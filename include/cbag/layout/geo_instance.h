#ifndef CBAG_LAYOUT_GEO_INSTANCE_H
#define CBAG_LAYOUT_GEO_INSTANCE_H

#include <cbag/layout/transformation.h>

namespace cbag {
namespace layout {

class geometry;
class rectangle;

class geo_instance {
  private:
    const geometry *master = nullptr;
    transformation xform;

  public:
    geo_instance();

    geo_instance(const geometry *master, transformation xform);

    bool empty() const;

    rectangle &get_bbox(rectangle &r) const;

    bool operator==(const geo_instance &rhs) const;
};

} // namespace layout
} // namespace cbag

#endif
