#ifndef CBAG_LAYOUT_BOUNDARY_H
#define CBAG_LAYOUT_BOUNDARY_H

#include <cbag/common/boundary_type.h>
#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

class boundary : public polygon {
  public:
    explicit inline boundary(point_vector_t data, boundary_type type)
        : polygon(std::move(data)), type(type) {}

  private:
    boundary_type type;
};

} // namespace layout
} // namespace cbag

#endif
