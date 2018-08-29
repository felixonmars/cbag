#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cbag/layout/geometry.h>
#include <cbag/layout/vector45.h>
#include <cbag/math/constexpr.h>

namespace cbag {
namespace layout {

const std::unordered_map<std::string, path_style> geometry::style_map = {
    {"truncate", psTruncate},
    {"extend", psExtend},
    {"round", psRound},
};

geometry::geometry(uint8_t mode) : mode(mode), view(make_union_view()) {}

rectangle geometry::get_bbox() const { return extents(view); }

polygon_ref<rectangle> geometry::add_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
    std::size_t idx = rect_set.size();
    rect_set.emplace_back(xl, yl, xh, yh);
    return {&rect_set, idx};
}

polygon_ref<polygon90> geometry::add_poly90(point_vector_t data) {
    std::size_t idx = poly90_set.size();
    mode = std::max(mode, 1_uc);
    poly90_set.emplace_back(std::move(data));
    return {&poly90_set, idx};
}

polygon_ref<polygon45> geometry::add_poly45(point_vector_t data) {
    std::size_t idx = poly45_set.size();
    mode = std::max(mode, 2_uc);
    poly45_set.emplace_back(std::move(data));
    return {&poly45_set, idx};
}

polygon_ref<polygon> geometry::add_poly(point_vector_t data) {
    std::size_t idx = poly_set.size();
    mode = std::max(mode, 3_uc);
    poly_set.emplace_back(std::move(data));
    return {&poly_set, idx};
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

constexpr double root2 = cbag::math::sqrt(2);

void add_path_points(point_vector_t &vec, coord_t x, coord_t y, path_style style,
                     offset_t half_width, const vector45 &p, const vector45 &n) {
    bool is_45 = p.is_45_or_invalid();
    uint32_t half_diag = round(half_width / root2);
    uint32_t w_main, w_norm;
    if (is_45) {
        w_main = half_diag;
        w_norm = half_width - half_diag;
    } else {
        w_main = half_width;
        w_norm = 2 * half_diag - half_width;
    }
    switch (style) {
    case psTruncate:
        vec.emplace_back(x + n.dx * w_main, y + n.dy * w_main);
        vec.emplace_back(x - n.dx * w_main, y - n.dy * w_main);
        break;
    case psExtend:
        vec.emplace_back(x - (p.dx - n.dx) * w_main, y - (p.dy - n.dy) * w_main);
        vec.emplace_back(x - (p.dx + n.dx) * w_main, y - (p.dy + n.dy) * w_main);
        break;
    default:
        switch (half_width) {
        case 1:
            if (is_45) {
                vec.emplace_back(x + n.dx, y + n.dy);
                vec.emplace_back(x - p.dx, y - p.dy);
                vec.emplace_back(x - n.dx, y - n.dy);
            } else {
                vec.emplace_back(x - (p.dx - n.dx), y - (p.dy - n.dy));
                vec.emplace_back(x - (p.dx + n.dx), y - (p.dy + n.dy));
            }
            break;
        case 2:
            if (is_45) {
                vec.emplace_back(x - (p.dx - n.dx), y - (p.dy - n.dy));
                vec.emplace_back(x - (p.dx + n.dx), y - (p.dy + n.dy));
            } else {
                vec.emplace_back(x + 2 * n.dx, y + 2 * n.dy);
                vec.emplace_back(x - 2 * p.dx, y - 2 * p.dy);
                vec.emplace_back(x - 2 * n.dx, y - 2 * n.dy);
            }
            break;
        default:
            vec.emplace_back(x - w_norm * p.dx + w_main * n.dx, y - w_norm * p.dy + w_main * n.dy);
            vec.emplace_back(x - w_main * p.dx + w_norm * n.dx, y - w_main * p.dy + w_norm * n.dy);
            vec.emplace_back(x - w_main * p.dx - w_norm * n.dx, y - w_main * p.dy - w_norm * n.dy);
            vec.emplace_back(x - w_norm * p.dx - w_main * n.dx, y - w_norm * p.dy - w_main * n.dy);
        }
    }
}

point_vector_t geometry::path_to_poly45(coord_t x0, coord_t y0, coord_t x1, coord_t y1,
                                        offset_t half_width, const char *style0,
                                        const char *style1) {

    vector45 p{x1 - x0, y1 - y0};

    // handle empty path
    if (half_width == 0 || (p.dx == 0 && p.dy == 0)) {
        return {};
    }
    // handle invalid path
    if (!p.valid()) {
        throw std::invalid_argument(fmt::format("path segment vector {} not valid", p));
    }

    path_style sty0 = style_map.at(style0);
    path_style sty1 = style_map.at(style1);

    // initialize point array, reserve space for worst case
    point_vector_t ans;
    ans.reserve(8);

    vector45 p_norm = p.get_norm();
    vector45 n_norm = p.get_rotate90();
    add_path_points(ans, x0, y0, sty0, half_width, p_norm, n_norm);
    p_norm.invert();
    n_norm.invert();
    add_path_points(ans, x1, y1, sty1, half_width, p_norm, n_norm);

    return ans;
}

} // namespace layout
} // namespace cbag
