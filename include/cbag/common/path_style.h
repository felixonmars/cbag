
#ifndef CBAG_COMMON_PATH_STYLE_H
#define CBAG_COMMON_PATH_STYLE_H

#include <cstdint>

namespace cbag {

enum class path_style : uint32_t { truncate = 0, extend = 1, round = 2, variable = 3 };

} // namespace cbag

#endif
