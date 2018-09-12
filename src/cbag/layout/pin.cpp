#include <cbag/layout/pin.h>

namespace cbag {
namespace layout {

pin::pin() = default;

pin::pin(coord_t xl, coord_t yl, coord_t xh, coord_t yh, const char *net, const char *label)
    : rectangle(xl, yl, xh, yh), net(net), label(label) {}

} // namespace layout
} // namespace cbag
