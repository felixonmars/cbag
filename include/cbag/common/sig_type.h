
#ifndef CBAG_COMMON_SIG_TYPE_H
#define CBAG_COMMON_SIG_TYPE_H

#include <cstdint>

namespace cbag {

enum class sig_type : uint32_t {
    signal = 0,
    power = 1,
    ground = 2,
    clock = 3,
    tieOff = 4,
    tieHi = 5,
    tieLo = 6,
    analog = 7,
    scan = 8,
    reset = 9
};

} // namespace cbag

#endif
