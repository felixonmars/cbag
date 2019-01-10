
#ifndef CBAG_ENUM_DIRECTION_H
#define CBAG_ENUM_DIRECTION_H

#include <cbag/common/typedefs.h>

namespace cbag {

enum class direction : enum_t {
    LOWER = 0,
    UPPER = 1,
};

inline enum_t to_int(direction vdir) { return static_cast<enum_t>(vdir); }

inline direction flip(direction dir) { return static_cast<direction>(1 - to_int(dir)); }

inline int get_adj_level(direction vdir, int level) {
    return level + 1 - (static_cast<int>(vdir) << 1);
}

} // namespace cbag

#endif
