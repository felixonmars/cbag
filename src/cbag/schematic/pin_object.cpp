#include <cbag/schematic/pin_object.h>

namespace cbag {
namespace sch {

pin_object::pin_object() {}

pin_object::pin_object(instance &&inst, term_attr &&attr) : inst(inst), attr(attr){};

} // namespace sch
} // namespace cbag
