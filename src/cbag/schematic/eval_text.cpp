#include <cbag/schematic/eval_text.h>

namespace cbag {
namespace sch {

eval_text::eval_text(lay_t lay, purp_t purp, std::string net, std::string text, text_align align,
                     orientation orient, font_t font, dist_t height, bool overbar, bool visible,
                     bool drafting, std::string eval)
    : text_t(lay, purp, std::move(net), std::move(text), align, orient, font, height, overbar,
             visible, drafting),
      evaluator(std::move(eval)) {}

} // namespace sch
} // namespace cbag
