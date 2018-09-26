#include <cbag/layout/blockage.h>

namespace cbag {
namespace layout {

blockage::blockage() = default;

blockage::blockage(const pt_vector &data, blockage_type type, lay_t layer)
    : type(type), layer(layer) {
    set(data.begin(), data.end());
}

} // namespace layout
} // namespace cbag
