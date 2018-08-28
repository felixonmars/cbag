/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_UNION_VIEW_H
#define CBAG_LAYOUT_UNION_VIEW_H

#include <variant>

namespace cbag {
namespace layout {

class rectangle;
class rectangle_view;
class polygon90_view;
class polygon45_view;
class polygon_view;

using union_view = std::variant<rectangle_view, polygon90_view, polygon45_view, polygon_view>;

rectangle extents(const union_view &view);

} // namespace layout
} // namespace cbag

#endif
