#include <cbag/schematic/text_base.h>

namespace cbag {
namespace sch {

text_base::text_base() = default;

text_base::text_base(lay_t lay, purp_t purp, std::string net, text_align align, orientation orient,
                     font_t font, dist_t height, bool overbar, bool visible, bool drafting)
    : shape_base(lay, purp, std::move(net)), origin(), alignment(align), orient(orient), font(font),
      height(height), overbar(overbar), visible(visible), drafting(drafting) {}

} // namespace sch
} // namespace cbag
