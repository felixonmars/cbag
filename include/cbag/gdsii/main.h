
#ifndef CBAG_GDSII_MAIN_H
#define CBAG_GDSII_MAIN_H

#include <string>
#include <unordered_map>

#include <cbag/logging/logging.h>

#include <cbag/util/io.h>

#include <cbag/layout/cellview_fwd.h>

namespace cbag {
namespace gdsii {

void write_gds_start(std::ofstream &stream, const std::string &lib_name, double resolution,
                     double user_unit);

void write_gds_stop(std::ofstream &stream);

void write_lay_cellview(spdlog::logger &logger, const std::string &lib_name,
                        const std::string &cell_name, const std::string &view_name,
                        const cbag::layout::cellview &cv,
                        const std::unordered_map<std::string, std::string> &rename_map);

template <class Vector>
void implement_gds(const std::string &fname, const std::string &lib_name, const std::string &view,
                   double resolution, double user_unit, const Vector &cv_list) {
    auto logger = get_cbag_logger();

    // get gds file stream
    auto stream = util::open_file_write(fname, true);
    write_gds_start(stream, lib_name, resolution, user_unit);

    std::unordered_map<std::string, std::string> rename_map{};
    for (const auto &cv_info : cv_list) {
        auto cv_ptr = cv_info.second;
        auto cell_name = cv_ptr->get_name();
        write_lay_cellview(*logger, lib_name, cv_info.first, view, *cv_ptr, rename_map);
        logger->info("cell name {} maps to {}", cell_name, cv_info.first);
        rename_map[cell_name] = cv_info.first;
    }

    write_gds_stop(stream);
}

} // namespace gdsii
} // namespace cbag

#endif
