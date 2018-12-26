#include <cbag/gdsii/main.h>
#include <cbag/gdsii/write.h>

#include <cbag/layout/cellview.h>

namespace cbag {
namespace gdsii {

void write_gds_start(spdlog::logger &logger, std::ofstream &stream, const std::string &lib_name,
                     double resolution, double user_unit) {
    write_header(logger, stream);
    write_bgn_lib(logger, stream);
    write_lib_name(logger, stream, lib_name);
    write_units(logger, stream, resolution, user_unit);
}

void write_gds_stop(spdlog::logger &logger, std::ofstream &stream) {
    write_end_lib(logger, stream);
    stream.close();
}

void write_lay_cellview(spdlog::logger &logger, const std::string &lib_name,
                        const std::string &cell_name, const cbag::layout::cellview &cv,
                        const std::unordered_map<std::string, std::string> &rename_map) {}

} // namespace gdsii
} // namespace cbag
