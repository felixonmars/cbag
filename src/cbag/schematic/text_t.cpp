#include <cbag/schematic/text_t.h>

namespace cbag {
namespace sch {

text_t::text_t() = default;

text_t::text_t(lay_t lay, purp_t purp, std::string net, std::string text, text_align align,
               orientation orient, font_t font, dist_t height, bool overbar, bool visible,
               bool drafting)
    : text_base(lay, purp, std::move(net), align, orient, font, height, overbar, visible, drafting),
      text(std::move(text)) {}

} // namespace sch
} // namespace cbag
