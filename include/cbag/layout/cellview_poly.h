#ifndef CBAG_LAYOUT_CELLVIEW_POLY_H
#define CBAG_LAYOUT_CELLVIEW_POLY_H

#include <cbag/layout/blockage.h>
#include <cbag/layout/boundary.h>
#include <cbag/layout/cv_obj_ref.h>
#include <cbag/layout/polygon45_fwd.h>
#include <cbag/layout/polygon90_fwd.h>
#include <cbag/layout/polygon_fwd.h>

namespace cbag {
namespace layout {

template <typename T, typename = IsPtVec<T>>
shape_ref<polygon90> add_poly90(cellview &cv, const std::string &layer, const std::string &purpose,
                                bool is_horiz, const T &data, bool commit) {
    polygon90 poly;
    return cv.add_poly90(layer, purpose, is_horiz, std::move(poly), commit);
}

} // namespace layout
} // namespace cbag

#endif
