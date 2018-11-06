#include <cbag/schematic/path.h>

namespace cbag {
namespace sch {

path::path() = default;

path::path(lay_t lay, purp_t purp, std::string net, dist_t width, uint32_t n, path_style style,
           dist_t begin_ext, dist_t end_ext)
    : shape_base(lay, purp, std::move(net)), width(width), style(style), begin_ext(begin_ext),
      end_ext(end_ext) {
    points.reserve(n);
}

} // namespace sch
} // namespace cbag
