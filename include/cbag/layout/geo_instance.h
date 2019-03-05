#ifndef CBAG_LAYOUT_GEO_INSTANCE_H
#define CBAG_LAYOUT_GEO_INSTANCE_H

#include <memory>

#include <cbag/common/transformation.h>

namespace cbag {

class box_t;

namespace layout {

class geo_iterator;
class geo_index;

class geo_instance {
  private:
    std::shared_ptr<const geo_index> master = nullptr;
    transformation xform;

  public:
    geo_instance();

    geo_instance(std::shared_ptr<const geo_index> master, transformation xform);

    bool empty() const;

    box_t get_bbox() const;

    geo_iterator begin_intersect(const box_t &r, offset_t spx, offset_t spy) const;

    bool operator==(const geo_instance &rhs) const;
};

} // namespace layout
} // namespace cbag

#endif
