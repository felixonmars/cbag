
#ifndef CBAG_COMMON_DESIGN_OUTPUT_H
#define CBAG_COMMON_DESIGN_OUTPUT_H

#include <cstdint>

namespace cbag {

enum class design_output : uint8_t {
    LAYOUT = 0,
    GDS = 1,
    SCHEMATIC = 2,
    YAML = 3,
    CDL = 4,
    VERILOG = 5,
};

} // namespace cbag

#endif
