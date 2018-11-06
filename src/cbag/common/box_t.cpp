
#include <cbag/common/box_t.h>

namespace cbag {

box_t::box_t() = default;

box_t::box_t(coord_t xl, coord_t yl, coord_t xh, coord_t yh) : xl(xl), yl(yl), xh(xh), yh(yh) {}

offset_t box_t::width() const { return xh - xl; }
offset_t box_t::height() const { return yh - yl; }

} // namespace cbag
