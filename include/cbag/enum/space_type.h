
#ifndef CBAG_ENUM_SPACE_TYPE_H
#define CBAG_ENUM_SPACE_TYPE_H

#include <cbag/common/typedefs.h>

namespace cbag {

enum class space_type : enum_t {
    DIFF_COLOR = 0,
    SAME_COLOR = 1,
    LINE_END = 2,
};

inline space_type get_space_type(bool same_color) { return static_cast<space_type>(same_color); }

} // namespace cbag

#endif
