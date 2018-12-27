#include <cbag/layout/via_wrapper.h>

namespace cbag {
namespace layout {

via_wrapper::via_wrapper() = default;

via_wrapper::via_wrapper(via v, bool add_layers, bool bot_horiz, bool top_horiz)
    : v(std::move(v)), add_layers(add_layers), bot_horiz(bot_horiz), top_horiz(top_horiz) {}

} // namespace layout
} // namespace cbag
