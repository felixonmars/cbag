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

struct donut : public shape_base {
  public:
    point center{};
    dist_t radius = 0;
    dist_t hole_radius = 0;

    donut();

    donut(lay_t lay, purp_t purp, std::string net, dist_t r, dist_t hole_r);

    donut(lay_t lay, purp_t purp, std::string net, dist_t r, dist_t hole_r, coord_t x, coord_t y);
};

} // namespace sch
} // namespace cbag

#endif
