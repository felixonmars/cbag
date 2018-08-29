#include <algorithm>

#include <cbag/layout/geometry.h>

namespace cbag {
namespace layout {

geometry::geometry(uint8_t mode) : mode(mode), view(make_union_view()) {}

rectangle geometry::get_bbox() const { return extents(view); }

polygon_ref<rectangle> geometry::add_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
    rect_set.emplace_back(xl, yl, xh, yh);
    return {&rect_set, rect_set.size() - 1};
}

polygon_ref<polygon90> geometry::add_poly90(point_vector_t data) {
    mode = std::max(mode, 1_uc);
    poly90_set.emplace_back(std::move(data));
    return {&poly90_set, poly90_set.size() - 1};
}

polygon_ref<polygon45> geometry::add_poly45(point_vector_t data) {
    mode = std::max(mode, 2_uc);
    poly45_set.emplace_back(std::move(data));
    return {&poly45_set, poly45_set.size() - 1};
}

polygon_ref<polygon> geometry::add_poly(point_vector_t data) {
    mode = std::max(mode, 3_uc);
    poly_set.emplace_back(std::move(data));
    return {&poly_set, poly_set.size() - 1};
}

union_view geometry::make_union_view() {
    switch (mode) {
    case 0:
        return rectangle_view(rect_set);
    case 1:
        return polygon90_view(poly90_set, rect_set);
    case 2:
        return polygon45_view(poly45_set, poly90_set, rect_set);
    default:
        return polygon_view(poly_set, poly45_set, poly90_set, rect_set);
    }
}

point_vector_t path_to_poly45(coord_t x0, coord_t y0, coord_t x1, coord_t y1, offset_t width,
                              const char *style0, const char *style1) {
    if (width == 0) {
        // zero condition
        return {};
    }

    offset_t dx = x1 - x0;
    offset_t dy = y1 - y0;
}

} // namespace layout
} // namespace cbag
