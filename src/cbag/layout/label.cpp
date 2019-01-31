#include <cbag/layout/label.h>

namespace cbag {
namespace layout {

label::label() = default;

label::label(layer_t key, transformation xform, std::string text, offset_t height)
    : lay_purp(std::move(key)), xform(std::move(xform)), text(std::move(text)), height(height) {}

layer_t label::get_key() const { return lay_purp; }

transformation label::get_xform() const { return xform; }

const std::string &label::get_text() const { return text; }

offset_t label::get_height() const noexcept { return height; }

bool label::operator==(const label &rhs) const noexcept {
    return lay_purp == rhs.lay_purp && xform == rhs.xform && text == rhs.text &&
           height == rhs.height;
}

} // namespace layout
} // namespace cbag
