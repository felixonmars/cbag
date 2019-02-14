#include <cbag/schematic/cellview_info.h>

namespace cbag {
namespace sch {

cellview_info::cellview_info() = default;

cellview_info::cellview_info(std::string lib_name, std::string cell_name, bool is_prim)
    : lib_name(std::move(lib_name)), cell_name(std::move(cell_name)), is_prim(is_prim) {}

} // namespace sch
} // namespace cbag
