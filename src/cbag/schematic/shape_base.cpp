#include <cbag/schematic/shape_base.h>

namespace cbag {
namespace sch {

shape_base::shape_base(lay_t lay, purp_t purp, std::string net)
    : layer(lay), purpose(purp), net(std::move(net)) {}

} // namespace sch
} // namespace cbag
