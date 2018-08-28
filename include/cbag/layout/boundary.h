#ifndef CBAG_LAYOUT_BOUNDARY_H
#define CBAG_LAYOUT_BOUNDARY_H

#include <cbag/common/boundary_type.h>
#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

class boundary : public polygon {
  private:
    boundary_type type;

  public:
    boundary(point_vector_t data, boundary_type type);
};

} // namespace layout
} // namespace cbag

#endif
