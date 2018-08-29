#include <cbag/schematic/arc.h>

namespace cbag {
namespace sch {

arc::arc() = default;

arc::arc(lay_t lay, purp_t purp, std::string net, double start, double stop)
    : shape_base(lay, purp, std::move(net)), ang_start(start), ang_stop(stop) {}

arc::arc(lay_t lay, purp_t purp, std::string net, double start, double stop, coord_t xl, coord_t yl,
         coord_t xh, coord_t yh)
    : shape_base(lay, purp, std::move(net)), ang_start(start), ang_stop(stop),
      bbox(xl, yl, xh, yh) {}

} // namespace sch
} // namespace cbag
