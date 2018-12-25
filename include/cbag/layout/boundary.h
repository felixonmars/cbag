#ifndef CBAG_LAYOUT_BOUNDARY_H
#define CBAG_LAYOUT_BOUNDARY_H

#include <cbag/enum/boundary_type.h>
#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

class boundary : public polygon {
  public:
    boundary_type type;

    explicit boundary(boundary_type type = boundary_type::PR);
};

} // namespace layout
} // namespace cbag

#endif
