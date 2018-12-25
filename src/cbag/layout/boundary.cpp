#include <cbag/layout/boundary.h>

namespace cbag {
namespace layout {

boundary::boundary(boundary_type type) noexcept : type(type) {}

boundary_type boundary::get_type() const noexcept { return type; }

} // namespace layout
} // namespace cbag
