/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_DONUT_H
#define CBAG_SCHEMATIC_DONUT_H

#include <cbag/common/point.h>
#include <cbag/schematic/shape_base.h>

namespace cbag {
namespace sch {

struct donut : shape_base {
    inline donut() : shape_base(), center(0, 0), radius(0), hole_radius(0) {}

    inline donut(lay_t lay, purp_t purp, std::string net, dist_t r, dist_t hole_r)
        : shape_base(lay, purp, std::move(net)), radius(r), hole_radius(hole_r) {}

    inline donut(lay_t lay, purp_t purp, std::string net, dist_t r, dist_t hole_r, coord_t x,
                 coord_t y)
        : shape_base(lay, purp, std::move(net)), center(x, y), radius(r), hole_radius(hole_r) {}

    point center;
    dist_t radius, hole_radius;
};

} // namespace sch
} // namespace cbag

#endif
