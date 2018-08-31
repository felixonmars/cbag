#ifndef CBAG_LAYOUT_BOUNDARY_H
#define CBAG_LAYOUT_BOUNDARY_H

#include <cbag/common/boundary_type.h>
#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

class pt_vector;

class boundary : public polygon {
  private:
    boundary_type type = boundary_type::PR;

  public:
    boundary(pt_vector data, boundary_type type);
};

} // namespace layout
} // namespace cbag

#endif
