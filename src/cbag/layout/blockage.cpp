#include <cbag/layout/blockage.h>

namespace cbag {
namespace layout {

blockage::blockage(point_vector_t data, blockage_type type)
    : polygon(std::move(data)), type(type) {}

} // namespace layout
} // namespace cbag
