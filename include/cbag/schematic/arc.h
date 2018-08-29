/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_ARC_H
#define CBAG_SCHEMATIC_ARC_H

#include <cbag/common/box_t.h>
#include <cbag/schematic/shape_base.h>

namespace cbag {
namespace sch {

struct arc : public shape_base {
  public:
    double ang_start = 0;
    double ang_stop = 0;
    box_t bbox;

    arc();

    arc(lay_t lay, purp_t purp, std::string net, double start, double stop);

    arc(lay_t lay, purp_t purp, std::string net, double start, double stop, coord_t xl, coord_t yl,
        coord_t xh, coord_t yh);
};

} // namespace sch
} // namespace cbag

#endif
