#ifndef CBAG_LAYOUT_POLYGON90_SET_H
#define CBAG_LAYOUT_POLYGON90_SET_H

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

using polygon90_set = boost::polygon::polygon_90_set_data<coord_t>;

} // namespace layout
} // namespace cbag

#endif
