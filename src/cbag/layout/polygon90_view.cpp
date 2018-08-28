#include <cbag/layout/polygon90_view.h>

namespace cbag {
namespace layout {

polygon90_view::polygon90_view(const polygon90_set &lval, const rectangle_set &rval)
    : joined_ra_range<polygon90_set, rectangle_set>(lval, rval) {}

} // namespace layout
} // namespace cbag
