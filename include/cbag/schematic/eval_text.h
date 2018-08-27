/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_EVAL_TEXT_H
#define CBAG_SCHEMATIC_EVAL_TEXT_H

#include <cbag/schematic/text_t.h>

namespace cbag {
namespace sch {

struct eval_text : text_t {
    inline eval_text() : text_t(), evaluator() {}

    inline eval_text(lay_t lay, purp_t purp, std::string net, std::string text, text_align align,
                     orientation orient, font_t font, dist_t height, bool overbar, bool visible,
                     bool drafting, std::string eval)
        : text_t(lay, purp, std::move(net), std::move(text), align, orient, font, height, overbar,
                 visible, drafting),
          evaluator(std::move(eval)) {}

    std::string evaluator;
};

} // namespace sch
} // namespace cbag

#endif
