/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_TEXT_H
#define CBAG_SCHEMATIC_TEXT_H

#include <string>

#include <cbag/schematic/text_base.h>

namespace cbag {
namespace sch {

struct text_t : public text_base {
  public:
    std::string text{};

    text_t();

    text_t(lay_t lay, purp_t purp, std::string net, std::string text, text_align align,
           orientation orient, font_t font, dist_t height, bool overbar, bool visible,
           bool drafting);
};

} // namespace sch
} // namespace cbag

#endif
