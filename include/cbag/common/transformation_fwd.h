
#ifndef CBAG_COMMON_TRANSFORMATION_FWD_H
#define CBAG_COMMON_TRANSFORMATION_FWD_H

#include <cbag/common/typedefs.h>

namespace boost {
namespace polygon {

template <typename T> class transformation;

} // namespace polygon
} // namespace boost

namespace cbag {

using transformation = boost::polygon::transformation<coord_t>;

} // namespace cbag

#endif
