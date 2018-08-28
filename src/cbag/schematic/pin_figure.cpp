#include <cbag/schematic/pin_figure.h>

namespace cbag {
namespace sch {

pin_figure::pin_figure() : stype(stSignal) {}

pin_figure::pin_figure(rectangle &&obj, sig_type stype) : obj(std::move(obj)), stype(stype) {}

pin_figure::pin_figure(pin_object &&obj, sig_type stype) : obj(std::move(obj)), stype(stype) {}

} // namespace sch
} // namespace cbag
