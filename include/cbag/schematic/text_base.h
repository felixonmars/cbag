/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_TEXT_BASE_H
#define CBAG_SCHEMATIC_TEXT_BASE_H

#include <cbag/common/point.h>
#include <cbag/common/typedefs.h>
#include <cbag/enum/font_t.h>
#include <cbag/enum/orientation.h>
#include <cbag/enum/text_align.h>
#include <cbag/schematic/shape_base.h>

namespace cbag {
namespace sch {

struct text_base : public shape_base {
  public:
    point origin;
    text_align alignment = text_align::centerCenter;
    orientation orient = oR0;
    font_t font = font_t::roman;
    dist_t height = 0;
    bool overbar = false;
    bool visible = true;
    bool drafting = false;

    text_base();

    text_base(lay_t lay, purp_t purp, std::string net, text_align align, orientation orient,
              font_t font, dist_t height, bool overbar, bool visible, bool drafting, coord_t x,
              coord_t y);
};

} // namespace sch
} // namespace cbag

#endif
