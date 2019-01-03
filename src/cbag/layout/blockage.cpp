#include <cbag/layout/blockage.h>

namespace cbag {
namespace layout {

blockage::blockage(blockage_type type, lay_t layer) noexcept : type(type) {
    layer = (type == blockage_type::placement) ? 0 : layer;
}

blockage_type blockage::get_type() const noexcept { return type; }

lay_t blockage::get_layer() const noexcept { return layer; }

bool blockage::operator==(const blockage &rhs) const noexcept {
    return polygon::operator==(rhs) && layer == rhs.layer && type == rhs.type;
}
} // namespace layout
} // namespace cbag
