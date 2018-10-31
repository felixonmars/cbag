/** \file read_oa.h
 *  \brief This file converts OpenAccess objects to CBAG data structure.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#ifndef CBAGOA_READ_OA_H
#define CBAGOA_READ_OA_H

#include <memory>

#include <oa/oaDesignDB.h>

#include <cbag/common/datatypes.h>
#include <cbag/schematic/shape_t.h>

// forward declare structures to reduce dependencies
namespace spdlog {
class logger;
}

namespace cbag {
namespace sch {
class instance;
class pin_figure;
class cellview;
} // namespace sch
} // namespace cbag

namespace cbagoa {

cbag::sch::cellview read_sch_cellview(const oa::oaCdbaNS &ns, spdlog::logger &logger,
                                      oa::oaDesign *p);

} // namespace cbagoa

#endif // CBAGOA_READ_OA_H
