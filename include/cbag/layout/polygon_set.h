#ifndef CBAG_LAYOUT_POLYGON_SET_H
#define CBAG_LAYOUT_POLYGON_SET_H

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

using polygon_set = boost::polygon::polygon_set_data<coord_t>;

} // namespace layout
} // namespace cbag

#endif
