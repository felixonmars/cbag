
#ifndef CBAG_COMMON_BOUNDARY_TYPE_H
#define CBAG_COMMON_BOUNDARY_TYPE_H

#include <cstdint>

namespace cbag {
// common primitive types

enum boundary_type : uint8_t {
    PR = 0,
    snap = 1,
};
} // namespace cbag

#endif
