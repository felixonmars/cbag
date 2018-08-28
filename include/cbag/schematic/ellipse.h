/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_ELLIPSE_H
#define CBAG_SCHEMATIC_ELLIPSE_H

#include <cbag/common/box_t.h>
#include <cbag/schematic/shape_base.h>

namespace cbag {
namespace sch {

struct ellipse : public shape_base {
  public:
    box_t bbox;

    ellipse() = default;

    ellipse(lay_t lay, purp_t purp, std::string net);

    ellipse(lay_t lay, purp_t purp, std::string net, coord_t xl, coord_t yl, coord_t xh,
            coord_t yh);
};

} // namespace sch
} // namespace cbag

#endif
