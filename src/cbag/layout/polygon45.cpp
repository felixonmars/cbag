#include <cbag/layout/polygon45.h>

namespace cbag {
namespace layout {

polygon45::polygon45() = default;

polygon45::polygon45(std::size_t n) : polygon(n) {}

polygon45::polygon45(pt_vector data, winding_dir wdir) : polygon(std::move(data), wdir) {}

} // namespace layout
} // namespace cbag
