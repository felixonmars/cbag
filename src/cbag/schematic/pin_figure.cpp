#include <cbag/schematic/pin_figure.h>

namespace cbag {
namespace sch {

pin_figure::pin_figure() = default;

pin_figure::pin_figure(rectangle &&obj, sig_type stype, term_type ttype)
    : obj(std::move(obj)), stype(stype), ttype(ttype) {}

pin_figure::pin_figure(pin_object &&obj, sig_type stype, term_type ttype)
    : obj(std::move(obj)), stype(stype), ttype(ttype) {}

} // namespace sch
} // namespace cbag
