#include <cbag/schematic/rectangle.h>

namespace cbag {
namespace sch {

rectangle::rectangle() = default;

rectangle::rectangle(lay_t lay, purp_t purp, std::string net)
    : shape_base(lay, purp, std::move(net)) {}

rectangle::rectangle(lay_t lay, purp_t purp, std::string net, coord_t xl, coord_t yl, coord_t xh,
                     coord_t yh)
    : shape_base(lay, purp, std::move(net)), bbox(xl, yl, xh, yh) {}

} // namespace sch
} // namespace cbag
