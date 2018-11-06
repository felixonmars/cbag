#include <cbag/schematic/polygon.h>

namespace cbag {
namespace sch {

polygon::polygon() = default;

polygon::polygon(lay_t lay, purp_t purp, std::string net, uint32_t n)
    : shape_base(lay, purp, std::move(net)) {
    points.reserve(n);
}

} // namespace sch
} // namespace cbag
