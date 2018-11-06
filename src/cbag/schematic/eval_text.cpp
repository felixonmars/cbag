#include <cbag/schematic/eval_text.h>

namespace cbag {
namespace sch {

eval_text::eval_text() = default;

eval_text::eval_text(lay_t lay, purp_t purp, std::string net, std::string text, text_align align,
                     orientation orient, font_t font, dist_t height, bool overbar, bool visible,
                     bool drafting, std::string eval, coord_t x, coord_t y)
    : text_t(lay, purp, std::move(net), std::move(text), align, orient, font, height, overbar,
             visible, drafting, x, y),
      evaluator(std::move(eval)) {}

} // namespace sch
} // namespace cbag
