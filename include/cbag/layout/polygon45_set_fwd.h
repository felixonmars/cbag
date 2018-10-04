#ifndef CBAG_LAYOUT_POLYGON45_SET_FWD_H
#define CBAG_LAYOUT_POLYGON45_SET_FWD_H

#include <cbag/common/typedefs.h>

namespace boost {
namespace polygon {
template <typename T> class polygon_45_set_data;
} // namespace polygon
} // namespace boost

namespace cbag {
namespace layout {

using polygon45_set = boost::polygon::polygon_45_set_data<coord_t>;

} // namespace layout
} // namespace cbag

#endif
