#ifndef CBAG_LAYOUT_POLYGON_FWD_H
#define CBAG_LAYOUT_POLYGON_FWD_H

#include <cbag/common/typedefs.h>

namespace boost {
namespace polygon {

template <typename T> class polygon_data;

} // namespace polygon
} // namespace boost

namespace cbag {
namespace layout {

using polygon = boost::polygon::polygon_data<coord_t>;

} // namespace layout
} // namespace cbag

#endif
