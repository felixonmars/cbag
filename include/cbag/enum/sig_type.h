
#ifndef CBAG_ENUM_SIG_TYPE_H
#define CBAG_ENUM_SIG_TYPE_H

#include <cbag/common/typedefs.h>

namespace cbag {

enum class sig_type : enum_t {
    signal = 0,
    power = 1,
    ground = 2,
    clock = 3,
    tieOff = 4,
    tieHi = 5,
    tieLo = 6,
    analog = 7,
    scan = 8,
    reset = 9,
};

} // namespace cbag

#endif
