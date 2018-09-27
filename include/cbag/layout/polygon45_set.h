#ifndef CBAG_LAYOUT_POLYGON45_SET_H
#define CBAG_LAYOUT_POLYGON45_SET_H

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

using polygon45_set = boost::polygon::polygon_45_set_data<coord_t>;

} // namespace layout
} // namespace cbag

#endif
