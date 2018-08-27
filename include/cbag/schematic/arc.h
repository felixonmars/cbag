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

struct arc : shape_base {
    inline arc() : shape_base(), ang_start(0), ang_stop(0), bbox() {}

    inline arc(lay_t lay, purp_t purp, std::string net, double start, double stop)
        : shape_base(lay, purp, std::move(net)), ang_start(start), ang_stop(stop), bbox() {}

    inline arc(lay_t lay, purp_t purp, std::string net, double start, double stop, coord_t xl,
               coord_t yl, coord_t xh, coord_t yh)
        : shape_base(lay, purp, std::move(net)), ang_start(start), ang_stop(stop),
          bbox(xl, yl, xh, yh) {}

    double ang_start, ang_stop;
    box_t bbox;
};

} // namespace sch
} // namespace cbag

#endif
