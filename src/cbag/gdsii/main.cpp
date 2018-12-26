#include <cbag/gdsii/main.h>
#include <cbag/gdsii/write.h>

#include <cbag/layout/cellview.h>

namespace cbag {
namespace gdsii {

void write_gds_start(std::ofstream &stream, const std::string &lib_name, double resolution,
                     double user_unit) {
    write_header(stream);
    write_bgn_lib(stream);
    write_lib_name(stream, lib_name);
    write_units(stream, resolution, user_unit);
}

void write_gds_stop(std::ofstream &stream) {
    write_end_lib(stream);
    stream.close();
}

void write_lay_cellview(spdlog::logger &logger, const std::string &lib_name,
                        const std::string &cell_name, const cbag::layout::cellview &cv,
                        const std::unordered_map<std::string, std::string> &rename_map) {}

} // namespace gdsii
} // namespace cbag
