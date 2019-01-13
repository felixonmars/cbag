
#ifndef CBAG_ENUM_ROUND_MODE_H
#define CBAG_ENUM_ROUND_MODE_H

#include <cbag/common/typedefs.h>

namespace cbag {

enum class round_mode : senum_t {
    LESS = -2,
    LESS_EQ = -1,
    NEAREST = 0,
    GREATER_EQ = 1,
    GREATER = 2,
    NONE = 3,
};

} // namespace cbag

#endif
