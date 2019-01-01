#ifndef CBAG_LAYOUT_VIA_PARAM_UTIL_H
#define CBAG_LAYOUT_VIA_PARAM_UTIL_H

#include <type_traits>

#include <cbag/enum/orient_2d.h>
#include <cbag/layout/via_param.h>

namespace cbag {
namespace layout {

vector get_arr_dim(const via_param &p);

offset_t get_metal_dim(const via_param &p, orient_2d orient, cnt_t level);

} // namespace layout
} // namespace cbag

#endif
