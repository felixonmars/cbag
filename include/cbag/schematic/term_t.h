/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SCHEMATIC_TERM_T_H
#define CBAG_SCHEMATIC_TERM_T_H

#include <string>
#include <utility>

#include <cbag/util/sorted_map.h>

namespace cbag {
namespace sch {

struct pin_figure;

using term_t = cbag::util::sorted_map<std::string, pin_figure>;

} // namespace sch
} // namespace cbag

#endif // CBAG_SCHEMATIC_CELLVIEWS_H
