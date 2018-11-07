#ifndef CBAG_LAYOUT_GEO_INSTANCE_H
#define CBAG_LAYOUT_GEO_INSTANCE_H

#include <cbag/common/transformation.h>

namespace cbag {

class box_t;

namespace layout {

class geo_iterator;
class geometry;

class geo_instance {
  private:
    const geometry *master = nullptr;
    transformation xform;

  public:
    geo_instance();

    geo_instance(const geometry *master, transformation xform);

    bool empty() const;

    box_t &get_bbox(box_t &r) const;

    geo_iterator begin_intersect(const box_t &r, offset_t spx, offset_t spy) const;

    geo_iterator end_intersect() const;

    bool operator==(const geo_instance &rhs) const;
};

} // namespace layout
} // namespace cbag

#endif
