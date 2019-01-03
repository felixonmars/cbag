#include <cbag/gdsii/read.h>
#include <cbag/gdsii/read_util.h>

namespace cbag {
namespace gdsii {

std::string read_gds_start(spdlog::logger &logger, std::ifstream &stream) {
    read_header(logger, stream);
    read_lib_begin(logger, stream);
    auto ans = read_lib_name(logger, stream);
    read_units(logger, stream);
    return ans;
}

layout::cellview read_lay_cellview(spdlog::logger &logger, std::ifstream &stream,
                                   const std::string &lib_name, const layout::tech &t) {
    auto cell_name = read_struct_name(logger, stream);
    layout::cellview ans{&t, std::move(cell_name), geometry_mode::POLY};

    read_struct_end(logger, stream);
    return ans;
}

} // namespace gdsii
} // namespace cbag
