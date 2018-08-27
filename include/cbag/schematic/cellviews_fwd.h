/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SCHEMATIC_CELLVIEWS_FWD_H
#define CBAG_SCHEMATIC_CELLVIEWS_FWD_H

#include <map>
#include <unordered_map>

namespace cbag {
namespace sch {

struct instance;
struct cellview;
struct cellview_info;
struct pin_figure;

using term_t = std::map<std::string, pin_figure>;

} // namespace sch
} // namespace cbag

#endif
