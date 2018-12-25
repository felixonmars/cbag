
#ifndef CBAG_ENUM_SPACE_TYPE_H
#define CBAG_ENUM_SPACE_TYPE_H

#include <cbag/common/typedefs.h>

namespace cbag {

enum class space_type : enum_t {
    LINE_END = 0,
    DIFF_COLOR = 1,
    SAME_COLOR = 2,
};

} // namespace cbag

#endif
