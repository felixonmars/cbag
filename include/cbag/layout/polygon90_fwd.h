#ifndef CBAG_LAYOUT_POLYGON90_FWD_H
#define CBAG_LAYOUT_POLYGON90_FWD_H

#include <cbag/common/typedefs.h>

namespace boost {
namespace polygon {

template <typename T> class polygon_90_data;

} // namespace polygon
} // namespace boost

namespace cbag {
namespace layout {

using polygon90 = boost::polygon::polygon_90_data<coord_t>;

} // namespace layout
} // namespace cbag

#endif
