/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_DATATYPES_H
#define CBAG_LAYOUT_DATATYPES_H

#include <variant>

#include <cbag/layout/polygon.h>
#include <cbag/layout/polygon45.h>
#include <cbag/layout/polygon45_view.h>
#include <cbag/layout/polygon90.h>
#include <cbag/layout/polygon90_view.h>
#include <cbag/layout/polygon_view.h>
#include <cbag/layout/rectangle.h>
#include <cbag/layout/rectangle_view.h>

namespace cbag {
namespace layout {

using union_view = std::variant<rectangle_view, polygon90_view, polygon45_view, polygon_view>;

} // namespace layout
} // namespace cbag

#endif
