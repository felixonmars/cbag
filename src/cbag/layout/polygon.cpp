#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

polygon::polygon() = default;

polygon::polygon(std::size_t n) : data() { data.reserve(n); }

polygon::polygon(point_vector_t data) : data(std::move(data)) {}

} // namespace layout
} // namespace cbag
