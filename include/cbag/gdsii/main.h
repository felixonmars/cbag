
#ifndef CBAG_GDSII_MAIN_H
#define CBAG_GDSII_MAIN_H

#include <string>
#include <unordered_map>

#include <cbag/logging/logging.h>

#include <cbag/util/io.h>

#include <cbag/layout/cellview_fwd.h>

namespace cbag {
namespace gdsii {

void write_header(std::ofstream &stream, const std::string &lib_name);

void write_footer(std::ofstream &stream);

void write_lay_cellview(spdlog::logger &logger, const std::string &lib_name,
                        const std::string &cell_name, const std::string &view_name,
                        const cbag::layout::cellview &cv,
                        const std::unordered_map<std::string, std::string> &rename_map);

template <class Vector>
void implement_lay_list(spdlog::logger &logger, const std::string &fname,
                        const std::string &lib_name, const std::string &view,
                        const Vector &cv_list) {
    // get gds file stream
    auto stream = util::open_file_write(fname, true);
    write_header(stream, lib_name);

    std::unordered_map<std::string, std::string> rename_map{};
    for (const auto &cv_info : cv_list) {
        write_lay_cellview(logger, lib_name, cv_info.first, view, *(cv_info.second), rename_map);
        logger.info("cell name {} maps to {}", cv_info.second->cell_name, cv_info.first);
        rename_map[cv_info.second->cell_name] = cv_info.first;
    }

    write_footer(stream);
}

} // namespace gdsii
} // namespace cbag

#endif
