#include <cbag/layout/via_wrapper.h>

namespace cbag {
namespace layout {

via_wrapper::via_wrapper() = default;

via_wrapper::via_wrapper(via &&v, bool add_layers) : v(std::move(v)), add_layers(add_layers) {}

} // namespace layout
} // namespace cbag
