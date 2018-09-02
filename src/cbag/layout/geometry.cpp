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

struct geometry::helper {
    static const std::unordered_map<std::string, end_style> style_map;

    static union_view make_union_view(const geometry &self) {
        switch (self.mode) {
        case 0:
            return rectangle_view(self.rect_set);
        case 1:
            return polygon90_view(self.poly90_set, self.rect_set);
        case 2:
            return polygon45_view(self.poly45_set, self.poly90_set, self.rect_set);
        default:
            return polygon_view(self.poly_set, self.poly45_set, self.poly90_set, self.rect_set);
        }
    }

    static end_style get_style(const char *style_str, offset_t half_width, bool is_45) {
        end_style ans = style_map.at(style_str);
        if (ans == end_style::round) {
            // handle degenerate cases
            switch (half_width) {
            case 1:
                return (is_45) ? end_style::triangle : end_style::extend;
            case 2:
                return (is_45) ? end_style::extend : end_style::triangle;
            default:
                return end_style::round;
            }
        }
        return ans;
    }
};

const std::unordered_map<std::string, end_style> geometry::helper::style_map = {
    {"truncate", end_style::truncate},
    {"extend", end_style::extend},
    {"round", end_style::round},
    {"triangle", end_style::triangle},
};

geometry::geometry(uint8_t mode) : mode(mode), view(helper::make_union_view(*this)) {}

rectangle geometry::get_bbox() const { return extents(view); }

polygon_ref<rectangle> geometry::add_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
    std::size_t idx = rect_set.size();
    rect_set.emplace_back(xl, yl, xh, yh);
    return {&rect_set, idx};
}

polygon_ref<polygon90> geometry::add_poly90(pt_vector data) {
    std::size_t idx = poly90_set.size();
    mode = std::max(mode, 1_uc);
    poly90_set.emplace_back(std::move(data));
    return {&poly90_set, idx};
}

polygon_ref<polygon45> geometry::add_poly45(pt_vector data) {
    std::size_t idx = poly45_set.size();
    mode = std::max(mode, 2_uc);
    poly45_set.emplace_back(std::move(data));
    return {&poly45_set, idx};
}

polygon_ref<polygon> geometry::add_poly(pt_vector data) {
    std::size_t idx = poly_set.size();
    mode = std::max(mode, 3_uc);
    poly_set.emplace_back(std::move(data));
    return {&poly_set, idx};
}

constexpr double root2 = cbag::math::sqrt(2);

void add_path_points(pt_vector &vec, coord_t x, coord_t y, const vector45 &p, const vector45 &n,
                     bool is_45, end_style style, uint32_t w_main, uint32_t w_norm) {
    switch (style) {
    case end_style::truncate: {
        uint32_t xw = n.dx * w_main;
        uint32_t yw = n.dy * w_main;
        vec.emplace_back(x + xw, y + yw);
        vec.emplace_back(x - xw, y - yw);
        break;
    }
    case end_style::extend:
        vec.emplace_back(x - (p.dx - n.dx) * w_main, y - (p.dy - n.dy) * w_main);
        vec.emplace_back(x - (p.dx + n.dx) * w_main, y - (p.dy + n.dy) * w_main);
        break;
    case end_style::triangle: {
        uint32_t xw = n.dx * w_main;
        uint32_t yw = n.dy * w_main;
        vec.emplace_back(x + xw, y + yw);
        vec.emplace_back(x - w_main * p.dx, y - w_main * p.dy);
        vec.emplace_back(x - xw, y - yw);
        break;
    }
    default: {
        uint32_t xnm = n.dx * w_main;
        uint32_t ynm = n.dy * w_main;
        uint32_t xpm = p.dx * w_main;
        uint32_t ypm = p.dy * w_main;
        uint32_t xnn = n.dx * w_norm;
        uint32_t ynn = n.dy * w_norm;
        uint32_t xpn = p.dx * w_norm;
        uint32_t ypn = p.dy * w_norm;
        vec.emplace_back(x - xpn + xnm, y - ypn + ynm);
        vec.emplace_back(x - xpm + xnn, y - ypm + ynn);
        vec.emplace_back(x - xpm - xnn, y - ypm - ynn);
        vec.emplace_back(x - xpn - xnm, y - ypn - ynm);
    }
    }
}

pt_vector geometry::path_to_poly45(coord_t x0, coord_t y0, coord_t x1, coord_t y1,
                                   offset_t half_width, const char *style0, const char *style1) {

    vector45 p{x1 - x0, y1 - y0};

    // handle empty path
    if (half_width == 0 || (p.dx == 0 && p.dy == 0)) {
        return {};
    }
    // handle invalid path
    if (!p.valid()) {
        throw std::invalid_argument(fmt::format("path segment vector {} not valid", p));
    }

    bool is_45 = p.is_45_or_invalid();
    end_style sty0 = helper::get_style(style0, half_width, is_45);
    end_style sty1 = helper::get_style(style1, half_width, is_45);

    // initialize point array, reserve space for worst case
    pt_vector ans(8);

    vector45 p_norm = p.get_norm();
    vector45 n_norm = p.get_rotate90();
    uint32_t half_diag = round(half_width / root2);
    uint32_t w_main, w_norm;
    if (is_45) {
        w_main = half_diag;
        w_norm = half_width - half_diag;
    } else {
        w_main = half_width;
        w_norm = 2 * half_diag - half_width;
    }

    add_path_points(ans, x0, y0, p_norm, n_norm, is_45, sty0, w_main, w_norm);
    p_norm.invert();
    n_norm.invert();
    add_path_points(ans, x1, y1, p_norm, n_norm, is_45, sty1, w_main, w_norm);

    return ans;
}

} // namespace layout
} // namespace cbag
