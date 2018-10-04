#ifndef CBAG_LAYOUT_POLYGON90_SET_FWD_H
#define CBAG_LAYOUT_POLYGON90_SET_FWD_H

#include <cbag/common/typedefs.h>

namespace boost {
namespace polygon {
template <typename T> class polygon_90_set_data;
} // namespace polygon
} // namespace boost

namespace cbag {
namespace layout {

using polygon90_set = boost::polygon::polygon_90_set_data<coord_t>;

} // namespace layout
} // namespace cbag

#endif
