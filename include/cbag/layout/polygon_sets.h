/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_POLYGON_SETS_H
#define CBAG_LAYOUT_POLYGON_SETS_H

#include <vector>

namespace cbag {
namespace layout {

class polygon;
class polygon45;
class polygon90;
class rectangle;

using polygon_set = std::vector<polygon>;
using polygon45_set = std::vector<polygon45>;
using polygon90_set = std::vector<polygon90>;
using rectangle_set = std::vector<rectangle>;

} // namespace layout
} // namespace cbag

#endif
