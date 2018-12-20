#ifndef CBAG_LAYOUT_BOUNDARY_H
#define CBAG_LAYOUT_BOUNDARY_H

#include <cbag/common/boundary_type.h>
#include <cbag/layout/polygon.h>
#include <cbag/layout/pt_vector.h>

namespace cbag {
namespace layout {

class boundary : public polygon {
  public:
    boundary_type type = boundary_type::PR;

    boundary();
    boundary(const pt_vector &data, boundary_type type);
};

} // namespace layout
} // namespace cbag

#endif
