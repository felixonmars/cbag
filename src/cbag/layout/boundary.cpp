#include <cbag/layout/boundary.h>

namespace cbag {
namespace layout {

boundary::boundary(boundary_type type) noexcept : type(type) {}

boundary_type boundary::get_type() const noexcept { return type; }

bool boundary::operator==(const boundary &rhs) const noexcept {
    return polygon::operator==(rhs) && type == rhs.type;
}

} // namespace layout
} // namespace cbag
