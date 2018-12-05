#include <cbag/layout/pin.h>

namespace cbag {
namespace layout {

pin::pin() = default;

pin::pin(coord_t xl, coord_t yl, coord_t xh, coord_t yh, std::string net, std::string label)
    : box_t(xl, yl, xh, yh), net(std::move(net)), label(std::move(label)) {}

} // namespace layout
} // namespace cbag
