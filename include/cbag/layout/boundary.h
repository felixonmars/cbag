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
    explicit boundary(boundary_type type = boundary_type::PR) noexcept;

    boundary_type get_type() const noexcept;

    bool operator==(const boundary &rhs) const noexcept;
};

} // namespace layout
} // namespace cbag

#endif
