#include <cbag/schematic/pin_object.h>

namespace cbag {
namespace sch {

pin_object::pin_object() = default;

pin_object::pin_object(instance &&inst, term_attr &&attr)
    : inst(std::move(inst)), attr(std::move(attr)) {}

} // namespace sch
} // namespace cbag
