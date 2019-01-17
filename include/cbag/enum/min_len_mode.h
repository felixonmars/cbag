
#ifndef CBAG_ENUM_MIN_LEN_MODE_H
#define CBAG_ENUM_MIN_LEN_MODE_H

#include <cbag/common/typedefs.h>

namespace cbag {

enum class min_len_mode : senum_t {
    LOWER = -1,
    MIDDLE = 0,
    UPPER = 1,
    NONE = 2,
};

} // namespace cbag

#endif
