#include <cbag/schematic/term_attr.h>

namespace cbag {
namespace sch {

term_attr::term_attr() = default;

term_attr::term_attr(term_attr_type attr_type, lay_t lay, purp_t purp, std::string net,
                     text_align align, orientation orient, font_t font, dist_t height,
                     text_disp_format format, bool overbar, bool visible, bool drafting, coord_t x,
                     coord_t y)
    : text_base(lay, purp, std::move(net), align, orient, font, height, overbar, visible, drafting,
                x, y),
      attr_type(attr_type), format(format) {}

} // namespace sch
} // namespace cbag
