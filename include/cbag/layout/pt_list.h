#ifndef CBAG_LAYOUT_PT_LIST_H
#define CBAG_LAYOUT_PT_LIST_H

#include <utility>
#include <vector>

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

namespace traits {
template <typename T> struct pt_list {};
} // namespace traits

template <typename T> using IsPtList = typename traits::pt_list<T>::coordinate_type;

} // namespace layout
} // namespace cbag

#endif
