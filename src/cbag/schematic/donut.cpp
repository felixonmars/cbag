#include <cbag/schematic/donut.h>

namespace cbag {
namespace sch {

donut::donut() = default;

donut::donut(lay_t lay, purp_t purp, std::string net, dist_t r, dist_t hole_r)
    : shape_base(lay, purp, std::move(net)), radius(r), hole_radius(hole_r) {}

donut::donut(lay_t lay, purp_t purp, std::string net, dist_t r, dist_t hole_r, coord_t x, coord_t y)
    : shape_base(lay, purp, std::move(net)), center{x, y}, radius(r), hole_radius(hole_r) {}

} // namespace sch
} // namespace cbag
