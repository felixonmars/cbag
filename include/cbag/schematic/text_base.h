/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_TEXT_BASE_H
#define CBAG_SCHEMATIC_TEXT_BASE_H

#include <cbag/common/font_t.h>
#include <cbag/common/orientation.h>
#include <cbag/common/point.h>
#include <cbag/common/text_align.h>
#include <cbag/common/typedefs.h>
#include <cbag/schematic/shape_base.h>

namespace cbag {
namespace sch {

struct text_base : shape_base {
    inline text_base()
        : shape_base(), origin(0, 0), alignment(taCC), orient(oR0), font(fRoman), height(0),
          overbar(false), visible(true), drafting(true) {}

    inline text_base(lay_t lay, purp_t purp, std::string net, text_align align, orientation orient,
                     font_t font, dist_t height, bool overbar, bool visible, bool drafting)
        : shape_base(lay, purp, std::move(net)), origin(), alignment(align), orient(orient),
          font(font), height(height), overbar(overbar), visible(visible), drafting(drafting) {}

    point origin;
    text_align alignment;
    orientation orient;
    font_t font;
    dist_t height;
    bool overbar, visible, drafting;
};

} // namespace sch
} // namespace cbag

#endif
