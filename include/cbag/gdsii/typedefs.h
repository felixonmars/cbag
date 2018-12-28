
#ifndef CBAG_GDSII_TYPEDEFS_H
#define CBAG_GDSII_TYPEDEFS_H

#include <cstdint>
#include <utility>

namespace cbag {
namespace gdsii {

using glay_t = uint16_t;
using gpurp_t = uint16_t;
using gds_layer_t = std::pair<glay_t, gpurp_t>;
using tval_t = uint16_t;

} // namespace gdsii
} // namespace cbag

#endif
