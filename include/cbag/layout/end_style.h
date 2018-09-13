#ifndef CBAG_LAYOUT_END_STYLE_H
#define CBAG_LAYOUT_END_STYLE_H

#include <cstdint>

namespace cbag {
namespace layout {

enum class end_style : uint8_t { truncate = 0, extend = 1, round = 2, triangle = 3 };

} // namespace layout
} // namespace cbag

#endif
