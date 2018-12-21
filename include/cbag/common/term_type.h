
#ifndef CBAG_COMMON_TERM_TYPE_H
#define CBAG_COMMON_TERM_TYPE_H

#include <cstdint>

namespace cbag {

enum class term_type : uint32_t {
    input = 0,
    output = 1,
    inout = 2,
    switch_type = 3,
    jumper = 4,
    unused = 5,
    tristate = 6
};

} // namespace cbag

#endif
