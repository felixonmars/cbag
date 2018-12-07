/** \file write_oa.h
 *  \brief This file writes CBAG objects to OpenAccess Database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#ifndef CBAG_OA_OA_WRITE_H
#define CBAG_OA_OA_WRITE_H

#include <unordered_map>

#include <oa/oaDesignDB.h>

#include <cbag/oa/typedef.h>

// forward declare structures to reduce dependencies
namespace spdlog {
class logger;
} // namespace spdlog

namespace cbagoa {

// Write method for schematic/symbol cell view

void write_sch_cellview(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                        spdlog::logger &logger, const std::string &lib_name,
                        const std::string &cell_name, const std::string &view_name, bool is_sch,
                        const cbag::sch::cellview &cv, const str_map_t *rename_map = nullptr);

void write_lay_cellview(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                        spdlog::logger &logger, const std::string &lib_name,
                        const std::string &cell_name, const std::string &view_name,
                        const cbag::layout::cellview &cv, oa::oaTech *tech,
                        const str_map_t *rename_map = nullptr);

} // namespace cbagoa

#endif // CBAGOA_WRITE_OA_H
