/** \file read_oa.h
 *  \brief This file converts OpenAccess objects to CBAG data structure.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#ifndef CBAGOA_OA_READ_H
#define CBAGOA_OA_READ_H

#include <oa/oaDesignDB.h>

// forward declare structures to reduce dependencies
namespace spdlog {
class logger;
}

namespace cbag {
namespace sch {
class cellview;
} // namespace sch
} // namespace cbag

namespace cbagoa {

cbag::sch::cellview read_sch_cellview(const oa::oaCdbaNS &ns, spdlog::logger &logger,
                                      oa::oaDesign *p);

} // namespace cbagoa

#endif // CBAGOA_READ_OA_H
