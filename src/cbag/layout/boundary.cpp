#include <cbag/layout/boundary.h>

namespace cbag {
namespace layout {

boundary::boundary(boundary_type type) : type(type) {}

boundary_type boundary::get_type() const { return type; }

} // namespace layout
} // namespace cbag
