#include <cbag/layout/polygon45_view.h>

namespace cbag {
namespace layout {

polygon45_view::polygon45_view(const polygon45_set &val0, const polygon90_set &val1,
                               const rectangle_set &val2)
    : joined_ra_range<polygon45_set, polygon90_view>(val0, view_90), view_90(val1, val2) {}

} // namespace layout
} // namespace cbag
