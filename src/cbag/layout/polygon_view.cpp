#include <cbag/layout/polygon_view.h>

namespace cbag {
namespace layout {

polygon_view::polygon_view(const polygon_set &val0, const polygon45_set &val1,
                           const polygon90_set &val2, const rectangle_set &val3)
    : joined_ra_range<polygon_view2, polygon90_view>(view_no_90, view_90), view_no_90(val0, val1),
      view_90(val2, val3) {}

} // namespace layout
} // namespace cbag
