#include <cbag/schematic/cellview_info.h>

namespace cbag {
namespace sch {

cellview_info::cellview_info() = default;

cellview_info::cellview_info(std::string name, bool is_prim) : cell_name(name), is_prim(is_prim) {}

} // namespace sch
} // namespace cbag
