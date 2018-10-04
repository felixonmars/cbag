#ifndef CBAG_LAYOUT_GEO_INDEX_IMPL_H
#define CBAG_LAYOUT_GEO_INDEX_IMPL_H

#include <boost/geometry/index/rtree.hpp>

#include <cbag/layout/geo_object.h>

namespace bgi = boost::geometry::index;

namespace cbag {
namespace layout {

using geo_index_impl = bgi::rtree<geo_object, bgi::quadratic<32, 16>>;
using geo_query_iter = geo_index_impl::const_query_iterator;

} // namespace layout
} // namespace cbag

#endif
