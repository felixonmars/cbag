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

struct polygon : public shape_base {
  public:
    point_array points;

    polygon();

    polygon(lay_t lay, purp_t purp, std::string net, uint32_t n);
};

} // namespace sch
} // namespace cbag

#endif
