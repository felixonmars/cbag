/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_POLYGON_H
#define CBAG_SCHEMATIC_POLYGON_H

#include <cbag/common/point_array.h>
#include <cbag/schematic/shape_base.h>

namespace cbag {
namespace sch {

struct polygon : shape_base {
    inline polygon() : shape_base(), points() {}

    inline polygon(lay_t lay, purp_t purp, std::string net, uint32_t n)
        : shape_base(lay, purp, std::move(net)), points(n) {}

    point_array points;
};

} // namespace sch
} // namespace cbag

#endif
