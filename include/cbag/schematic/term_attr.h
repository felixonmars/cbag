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

struct term_attr : public text_base {
  public:
    term_attr_type attr_type = tatName;
    text_disp_format format = tdfNameValue;

    term_attr();

    term_attr(term_attr_type attr_type, lay_t lay, purp_t purp, std::string net, text_align align,
              orientation orient, font_t font, dist_t height, text_disp_format format, bool overbar,
              bool visible, bool drafting);
};

} // namespace sch
} // namespace cbag

#endif
