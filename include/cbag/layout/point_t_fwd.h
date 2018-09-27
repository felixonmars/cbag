#ifndef CBAG_LAYOUT_POINT_T_FWD_H
#define CBAG_LAYOUT_POINT_T_FWD_H

#include <cbag/common/typedefs.h>

namespace boost {
namespace polygon {

template <typename T> class point_data;

} // namespace polygon
} // namespace boost

namespace cbag {
namespace layout {

using point_t = boost::polygon::point_data<coord_t>;

} // namespace layout
} // namespace cbag

#endif
