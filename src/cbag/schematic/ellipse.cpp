#include <cbag/schematic/ellipse.h>

namespace cbag {
namespace sch {

ellipse::ellipse(lay_t lay, purp_t purp, std::string net) : shape_base(lay, purp, std::move(net)) {}

ellipse::ellipse(lay_t lay, purp_t purp, std::string net, coord_t xl, coord_t yl, coord_t xh,
                 coord_t yh)
    : shape_base(lay, purp, std::move(net)), bbox(xl, yl, xh, yh) {}

} // namespace sch
} // namespace cbag
