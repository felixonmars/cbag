#ifndef CBAG_OA_OA_WRITE_LIB_H
#define CBAG_OA_OA_WRITE_LIB_H

#include <cbag/logging/spdlog.h>

#include <cbag/schematic/cellview_fwd.h>

#include <cbag/oa/oa_util.h>
#include <cbag/oa/oa_write.h>
#include <cbag/oa/typedef.h>

namespace cbagoa {

template <class Vector>
void implement_sch_list(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                        spdlog::logger &logger, const std::string &lib_name,
                        const std::string &sch_view, const std::string &sym_view,
                        const Vector &cv_list) {
    try {
        str_map_t rename_map;

        for (const auto &cv_info : cv_list) {
            const std::string &name = cv_info.first;
            const auto cv_ptr = cv_info.second.first;
            write_sch_cellview(ns_native, ns, logger, lib_name, name, sch_view, true, *cv_ptr,
                               &rename_map);
            if (cv_ptr->sym_ptr != nullptr && !sym_view.empty()) {
                write_sch_cellview(ns_native, ns, logger, lib_name, name, sym_view, false,
                                   *(cv_ptr->sym_ptr), &rename_map);
            }
            logger.info("cell name {} maps to {}", cv_ptr->cell_name, name);
            rename_map[cv_ptr->cell_name] = name;
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
