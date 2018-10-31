/** \file write_oa.h
 *  \brief This file writes CBAG objects to OpenAccess Database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#ifndef CBAGOA_OA_WRITE_H
#define CBAGOA_OA_WRITE_H

#include <unordered_map>

#include <oa/oaDesignDB.h>

// forward declare structures to reduce dependencies
namespace spdlog {
class logger;
} // namespace spdlog

namespace cbag {
namespace sch {
class cellview;
} // namespace sch

namespace layout {
class cellview;
} // namespace layout
} // namespace cbag

namespace cbagoa {

using str_map_t = std::unordered_map<std::string, std::string>;

// Write method for schematic/symbol cell view

void write_sch_cellview(const oa::oaCdbaNS &ns, spdlog::logger &logger,
                        const cbag::sch::cellview &cv, oa::oaDesign *dsn, bool is_sch,
                        const str_map_t *rename_map = nullptr);

void write_lay_cellview(const oa::oaCdbaNS &ns, spdlog::logger &logger,
                        const cbag::layout::cellview &cv, oa::oaDesign *dsn, oa::oaTech *tech,
                        const str_map_t *rename_map = nullptr);

} // namespace cbagoa

#endif // CBAGOA_WRITE_OA_H