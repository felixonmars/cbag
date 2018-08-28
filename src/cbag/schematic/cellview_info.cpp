#include <cbag/schematic/cellview_info.h>

namespace cbag {
namespace sch {

cellview_info::cellview_info() = default;

cellview_info::cellview_info(std::string name, size_t num_in, size_t num_out, size_t num_inout,
                             bool is_prim)
    : cell_name(name), is_prim(is_prim) {
    in_terms.reserve(num_in);
    out_terms.reserve(num_out);
    io_terms.reserve(num_inout);
}

} // namespace sch
} // namespace cbag
