
#ifndef CBAG_COMMON_DESIGN_OUTPUT_H
#define CBAG_COMMON_DESIGN_OUTPUT_H

#include <cbag/common/typedefs.h>

namespace cbag {

enum class design_output : enum_t {
    LAYOUT = 0,
    GDS = 1,
    SCHEMATIC = 2,
    YAML = 3,
    CDL = 4,
    VERILOG = 5,
    SYSVERILOG = 6,
};

} // namespace cbag

#endif
