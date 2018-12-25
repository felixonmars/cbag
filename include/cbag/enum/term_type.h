
#ifndef CBAG_ENUM_TERM_TYPE_H
#define CBAG_ENUM_TERM_TYPE_H

#include <cbag/common/typedefs.h>

namespace cbag {

enum class term_type : enum_t {
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
