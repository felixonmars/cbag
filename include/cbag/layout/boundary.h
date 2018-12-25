#ifndef CBAG_LAYOUT_BOUNDARY_H
#define CBAG_LAYOUT_BOUNDARY_H

#include <cbag/enum/boundary_type.h>
#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

class boundary : public polygon {
  private:
    boundary_type type;

  public:
    explicit boundary(boundary_type type = boundary_type::PR);

    boundary_type get_type() const;
};

} // namespace layout
} // namespace cbag

#endif
