#include <cbag/schematic/line.h>

namespace cbag {
namespace sch {

line::line(lay_t lay, purp_t purp, std::string net, uint32_t n)
    : shape_base(lay, purp, std::move(net)), points(n) {}

} // namespace sch
} // namespace cbag
