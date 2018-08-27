/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_TERM_ATTR_H
#define CBAG_SCHEMATIC_TERM_ATTR_H

#include <cbag/common/term_attr_type.h>
#include <cbag/common/text_disp_format.h>
#include <cbag/schematic/text_base.h>

namespace cbag {
namespace sch {

struct term_attr : text_base {
    inline term_attr() : text_base(), attr_type(tatName), format(tdfNameValue) {}

    inline term_attr(term_attr_type attr_type, lay_t lay, purp_t purp, std::string net,
                     text_align align, orientation orient, font_t font, dist_t height,
                     text_disp_format format, bool overbar, bool visible, bool drafting)
        : text_base(lay, purp, std::move(net), align, orient, font, height, overbar, visible,
                    drafting),
          attr_type(attr_type), format(format) {}

    term_attr_type attr_type;
    text_disp_format format;
};

} // namespace sch
} // namespace cbag

#endif
