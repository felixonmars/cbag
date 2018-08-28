/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_RECTANGLE_H
#define CBAG_SCHEMATIC_RECTANGLE_H

#include <cbag/common/box_t.h>
#include <cbag/schematic/shape_base.h>

namespace cbag {
namespace sch {

struct rectangle : public shape_base {
  public:
    box_t bbox;

    rectangle() = default;

    rectangle(lay_t lay, purp_t purp, std::string net);

    rectangle(lay_t lay, purp_t purp, std::string net, coord_t xl, coord_t yl, coord_t xh,
              coord_t yh);
};

} // namespace sch
} // namespace cbag

#endif
