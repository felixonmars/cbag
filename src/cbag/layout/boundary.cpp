#include <cbag/layout/boundary.h>

namespace cbag {
namespace layout {

boundary::boundary() = default;

boundary::boundary(const pt_vector &data, boundary_type type) : type(type) {
    set(data.begin(), data.end());
}

} // namespace layout
} // namespace cbag
