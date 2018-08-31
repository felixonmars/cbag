#include <cbag/layout/boundary.h>

namespace cbag {
namespace layout {

boundary::boundary(pt_vector data, boundary_type type) : polygon(std::move(data)), type(type) {}

} // namespace layout
} // namespace cbag
