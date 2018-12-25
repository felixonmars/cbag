#include <cbag/gdsii/main.h>
#include <cbag/gdsii/write.h>

#include <cbag/layout/cellview.h>

namespace cbag {
namespace gdsii {

void write_gds_start(std::ofstream &stream, const std::string &lib_name) { write_header(stream); }

void write_gds_stop(std::ofstream &stream) { stream.close(); }

void write_lay_cellview(spdlog::logger &logger, const std::string &lib_name,
                        const std::string &cell_name, const std::string &view_name,
                        const cbag::layout::cellview &cv,
                        const std::unordered_map<std::string, std::string> &rename_map) {}

} // namespace gdsii
} // namespace cbag
