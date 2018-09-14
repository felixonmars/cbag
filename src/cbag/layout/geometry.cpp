#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cbag/layout/geometry.h>
#include <cbag/layout/path_ref.h>
#include <cbag/layout/vector45.h>
#include <cbag/layout/vector_obj_ref.h>
#include <cbag/math/constexpr.h>

namespace cbag {
namespace layout {

struct geometry::helper {
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
};

geometry::geometry(uint8_t mode) : mode(mode), view(helper::make_union_view(*this)) {}

rectangle geometry::get_bbox() const { return extents(view); }

vector_obj_ref<rectangle> geometry::add_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
    std::size_t idx = rect_set.size();
    rect_set.emplace_back(xl, yl, xh, yh);
    return {&rect_set, idx};
}

vector_obj_ref<polygon90> geometry::add_poly90(pt_vector data) {
    std::size_t idx = poly90_set.size();
    mode = std::max(mode, 1_uc);
    poly90_set.emplace_back(std::move(data));
    return {&poly90_set, idx};
}

vector_obj_ref<polygon45> geometry::add_poly45(pt_vector data) {
    std::size_t idx = poly45_set.size();
    mode = std::max(mode, 2_uc);
    poly45_set.emplace_back(std::move(data));
    return {&poly45_set, idx};
}

vector_obj_ref<polygon> geometry::add_poly(pt_vector data) {
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

end_style get_style(end_style ans, offset_t half_width, bool is_45) {
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

pt_vector path_to_poly45(coord_t x0, coord_t y0, coord_t x1, coord_t y1, offset_t half_width,
                         end_style sty0, end_style sty1) {
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

path_ref geometry::add_path(const pt_vector &data, offset_t half_width, uint8_t style0,
                            uint8_t style1, uint8_t stylem) {
    pt_vector::size_type n = data.size();
    if (n < 2) {
        throw std::invalid_argument(fmt::format("Cannot draw path with less than 2 points."));
    }

    std::size_t start = poly45_set.size();
    mode = std::max(mode, 2_uc);

    auto s0 = static_cast<end_style>(style0);
    auto s1 = static_cast<end_style>(style1);
    auto sm = static_cast<end_style>(stylem);

    for (pt_vector::size_type istart = 0; istart < n - 1; ++istart) {
        end_style c0 = (istart == 0) ? s0 : sm;
        end_style c1 = (istart == n - 2) ? s1 : sm;
        poly45_set.emplace_back(path_to_poly45(data[istart].x(), data[istart].y(),
                                               data[istart + 1].x(), data[istart + 1].y(),
                                               half_width, c0, c1));
    }

    return {&poly45_set, start, start + n - 1};
}

path_ref geometry::add_path45_bus(const pt_vector &data, const std::vector<offset_t> &widths,
                                  const std::vector<offset_t> &spaces, uint8_t style0,
                                  uint8_t style1, uint8_t stylem) {

    pt_vector::size_type n_pts = data.size();
    if (n_pts < 2) {
        throw std::invalid_argument(fmt::format("Cannot draw path with less than 2 points."));
    }
    std::size_t n_paths = widths.size();
    if (n_paths != spaces.size() + 1) {
        throw std::invalid_argument(fmt::format("invalid size for path bus widths/spaces."));
    }

    // compute total width
    offset_t tot_width = 0;
    for (offset_t val : widths) {
        tot_width += val;
    }
    for (offset_t val : spaces) {
        tot_width += val;
    }

    // compute deltas
    std::vector<offset_t> deltas;
    deltas.reserve(n_paths);
    deltas[0] = (-tot_width + widths[0]) / 2;
    for (std::size_t idx = 1; idx < n_paths; ++idx) {
        deltas[idx] = deltas[idx - 1] + spaces[idx - 1] + (widths[idx - 1] + widths[idx]) / 2;
    }

    // get starting index and update mode
    std::size_t start = poly45_set.size();
    mode = std::max(mode, 2_uc);

    // get initial points
    pt_vector prev_pts;
    prev_pts.reserve(n_paths);
    coord_t x0 = data[0].x();
    coord_t y0 = data[0].y();
    vector45 s0{data[1].x() - x0, data[1].y() - y0};
    s0.rotate90_norm();
    if (s0.is_45_or_invalid()) {
        for (std::size_t idx = 0; idx < n_paths; ++idx) {
            int32_t scale = round(deltas[idx] / root2);
            prev_pts.emplace_back(x0 + s0.dx * scale, y0 + s0.dy * scale);
        }
    } else {
        for (std::size_t idx = 0; idx < n_paths; ++idx) {
            int32_t scale = deltas[idx];
            prev_pts.emplace_back(x0 + s0.dx * scale, y0 + s0.dy * scale);
        }
    }

    // add intermediate path segments
    auto sty1 = static_cast<end_style>(stylem);
    for (pt_vector::size_type nidx = 2; nidx < n_pts; ++nidx) {
        auto sty0 = static_cast<end_style>((nidx == 2) ? style0 : stylem);

        coord_t xc = data[nidx - 1].x();
        coord_t yc = data[nidx - 1].y();
        s0.dx = xc - data[nidx - 2].x();
        s0.dy = yc - data[nidx - 2].y();
        vector45 s1{data[nidx].x() - xc, data[nidx].y() - yc};
        s0.normalize();
        s1.normalize();
        vector45 dir1 = s1.get_rotate90();
        if (dir1.is_45_or_invalid()) {
            for (std::size_t idx = 0; idx < n_paths; ++idx) {
                int32_t scale = round(deltas[idx] / root2);
                coord_t prevx = prev_pts[idx].x();
                coord_t prevy = prev_pts[idx].y();
                coord_t pdx = xc + dir1.dx * scale - prevx;
                coord_t pdy = yc + dir1.dy * scale - prevy;
                int32_t k = (pdx * s1.dy - pdy * s1.dx) / (s0.dx * s1.dy - s0.dy * s1.dx);
                coord_t newx = prevx + k * s0.dx;
                coord_t newy = prevy + k * s0.dy;
                poly45_set.emplace_back(
                    path_to_poly45(prevx, prevy, newx, newy, widths[idx] / 2, sty0, sty1));
                prev_pts[idx].set(newx, newy);
            }
        } else {
            for (std::size_t idx = 0; idx < n_paths; ++idx) {
                int32_t scale = deltas[idx];
                coord_t prevx = prev_pts[idx].x();
                coord_t prevy = prev_pts[idx].y();
                coord_t pdx = xc + dir1.dx * scale - prevx;
                coord_t pdy = yc + dir1.dy * scale - prevy;
                int32_t k = (pdx * s1.dy - pdy * s1.dx) / (s0.dx * s1.dy - s0.dy * s1.dx);
                coord_t newx = prevx + k * s0.dx;
                coord_t newy = prevy + k * s0.dy;
                poly45_set.emplace_back(
                    path_to_poly45(prevx, prevy, newx, newy, widths[idx] / 2, sty0, sty1));
                prev_pts[idx].set(newx, newy);
            }
        }
    }

    // add last path segment
    auto sty0 = static_cast<end_style>((n_pts == 2) ? style0 : stylem);
    sty1 = static_cast<end_style>(style1);
    x0 = data[n_pts - 1].x();
    y0 = data[n_pts - 1].y();
    s0.dx = x0 - data[n_pts - 2].x();
    s0.dy = y0 - data[n_pts - 2].y();
    s0.rotate90_norm();
    if (s0.is_45_or_invalid()) {
        for (std::size_t idx = 0; idx < n_paths; ++idx) {
            int32_t scale = round(deltas[idx] / root2);
            poly45_set.emplace_back(path_to_poly45(prev_pts[idx].x(), prev_pts[idx].y(),
                                                   x0 + s0.dx * scale, y0 + s0.dy * scale,
                                                   widths[idx] / 2, sty0, sty1));
        }
    } else {
        for (std::size_t idx = 0; idx < n_paths; ++idx) {
            int32_t scale = deltas[idx];
            poly45_set.emplace_back(path_to_poly45(prev_pts[idx].x(), prev_pts[idx].y(),
                                                   x0 + s0.dx * scale, y0 + s0.dy * scale,
                                                   widths[idx] / 2, sty0, sty1));
        }
    }

    // return
    return {&poly45_set, start, poly45_set.size()};
}

} // namespace layout
} // namespace cbag
