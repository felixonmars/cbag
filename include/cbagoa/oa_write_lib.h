#ifndef CBAGOA_OA_WRITE_LIB_H
#define CBAGOA_OA_WRITE_LIB_H

#include <cbag/logging/spdlog.h>

#include <cbag/schematic/cellview_fwd.h>

#include <cbagoa/oa_util.h>
#include <cbagoa/oa_write.h>

namespace cbagoa {

using sch_cv_info = std::pair<std::string, cbag::sch::cellview *>;
using lay_cv_info = std::pair<std::string, cbag::layout::cellview *>;

template <class Vector>
void implement_sch_list(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                        spdlog::logger &logger, const std::string &lib_name,
                        const std::string &sch_view, const std::string &sym_view,
                        const Vector &cv_list) {
    try {
        str_map_t rename_map;

        for (const auto &cv_info : cv_list) {
            write_sch_cellview(ns_native, ns, logger, lib_name, cv_info.first, sch_view, true,
                               *(cv_info.second), &rename_map);
            if (cv_info.second->sym_ptr != nullptr && !sym_view.empty()) {
                write_sch_cellview(ns_native, ns, logger, lib_name, cv_info.first, sym_view, false,
                                   *(cv_info.second->sym_ptr));
            }
            logger.info("cell name {} maps to {}", cv_info.second->cell_name, cv_info.first);
            rename_map[cv_info.second->cell_name] = cv_info.first;
        }
    } catch (...) {
        handle_oa_exceptions(logger);
    }
}

template <class Vector>
void implement_lay_list(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                        spdlog::logger &logger, const std::string &lib_name,
                        const std::string &view, const Vector &cv_list) {
    try {
        str_map_t rename_map;
        auto *tech_ptr = read_tech(ns_native, lib_name);

        for (const auto &cv_info : cv_list) {
            write_lay_cellview(ns_native, ns, logger, lib_name, cv_info.first, view,
                               *(cv_info.second), tech_ptr, &rename_map);
            logger.info("cell name {} maps to {}", cv_info.second->cell_name, cv_info.first);
            rename_map[cv_info.second->cell_name] = cv_info.first;
        }
    } catch (...) {
        handle_oa_exceptions(logger);
    }
}

} // namespace cbagoa

#endif
