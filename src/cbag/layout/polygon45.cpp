#include <cbag/layout/polygon45.h>

namespace cbag {
namespace layout {

polygon45::polygon45() = default;

polygon45::polygon45(std::size_t n) : polygon(n) {}

polygon45::polygon45(point_vector_t data) : polygon(std::move(data)) {}

} // namespace layout
} // namespace cbag
