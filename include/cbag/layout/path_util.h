#ifndef CBAG_LAYOUT_PATH_UTIL_H
#define CBAG_LAYOUT_PATH_UTIL_H

#include <cbag/layout/cv_obj_ref.h>
#include <cbag/layout/end_style.h>
#include <cbag/layout/polygon45_fwd.h>
#include <cbag/layout/polygon45_set_fwd.h>
#include <cbag/layout/pt_vector.h>
#include <cbag/layout/vector45.h>
#include <cbag/math/constexpr.h>

namespace cbag {
namespace layout {

constexpr double root2 = cbag::math::sqrt(2);

pt_vector path_to_poly45(coord_t x0, coord_t y0, coord_t x1, coord_t y1, offset_t half_width,
                         end_style sty0, end_style sty1);

template <typename T, typename = IsPtList<T>>
polygon45_set make_path(const T &data, offset_t half_width, uint8_t style0, uint8_t style1,
                        uint8_t stylem) {
    auto n = traits::pt_list<T>::size(data);
    if (n < 2) {
        throw std::invalid_argument("Cannot draw path with less than 2 points.");
    }

    auto s0 = static_cast<end_style>(style0);
    auto s1 = static_cast<end_style>(style1);
    auto sm = static_cast<end_style>(stylem);

    polygon45_set ans;
    polygon45 tmp;
    for (pt_vector::size_type istart = 0; istart < n - 1; ++istart) {
        end_style c0 = (istart == 0) ? s0 : sm;
        end_style c1 = (istart == n - 2) ? s1 : sm;
        auto cx = traits::pt_list<T>::x(data, istart);
        auto cy = traits::pt_list<T>::y(data, istart);
        auto nx = traits::pt_list<T>::x(data, istart + 1);
        auto ny = traits::pt_list<T>::y(data, istart + 1);
        pt_vector pt_vec = path_to_poly45(cx, cy, nx, ny, half_width, c0, c1);
        tmp.set(pt_vec.begin(), pt_vec.end());
        ans.insert(tmp);
    }

    return ans;
}

template <typename T, typename L, typename = IsPtList<T>>
polygon45_set make_path45_bus(const T &data, const L &widths, const L &spaces, uint8_t style0,
                              uint8_t style1, uint8_t stylem) {
    auto n_pts = traits::pt_list<T>::size(data);
    if (n_pts < 2) {
        throw std::invalid_argument("Cannot draw path with less than 2 points.");
    }
    auto n_paths = widths.size();
    if (n_paths != spaces.size() + 1) {
        throw std::invalid_argument("invalid size for path bus widths/spaces.");
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

    // get initial points
    pt_vector prev_pts;
    prev_pts.reserve(n_paths);
    auto x0 = traits::pt_list<T>::x(data, 0);
    auto y0 = traits::pt_list<T>::y(data, 0);
    auto x1 = traits::pt_list<T>::x(data, 1);
    auto y1 = traits::pt_list<T>::y(data, 1);
    vector45 s0{x1 - x0, y1 - y0};
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
    polygon45_set ans;
    polygon45 tmp;
    auto sty1 = static_cast<end_style>(stylem);
    for (pt_vector::size_type nidx = 2; nidx < n_pts; ++nidx) {
        auto sty0 = static_cast<end_style>((nidx == 2) ? style0 : stylem);

        auto xc = traits::pt_list<T>::x(data, nidx);
        auto yc = traits::pt_list<T>::y(data, nidx);
        auto xp1 = traits::pt_list<T>::x(data, nidx - 1);
        auto yp1 = traits::pt_list<T>::y(data, nidx - 1);
        auto xp2 = traits::pt_list<T>::x(data, nidx - 2);
        auto yp2 = traits::pt_list<T>::y(data, nidx - 2);
        s0.dx = xp1 - xp2;
        s0.dy = yp1 - yp2;
        vector45 s1{xc - xp1, yc - yp1};
        s0.normalize();
        s1.normalize();
        vector45 dir1 = s1.get_rotate90();
        if (dir1.is_45_or_invalid()) {
            for (std::size_t idx = 0; idx < n_paths; ++idx) {
                int32_t scale = round(deltas[idx] / root2);
                auto prevx = traits::pt_list<pt_vector>::x(prev_pts, idx);
                auto prevy = traits::pt_list<pt_vector>::y(prev_pts, idx);
                coord_t pdx = xc + dir1.dx * scale - prevx;
                coord_t pdy = yc + dir1.dy * scale - prevy;
                int32_t k = (pdx * s1.dy - pdy * s1.dx) / (s0.dx * s1.dy - s0.dy * s1.dx);
                coord_t newx = prevx + k * s0.dx;
                coord_t newy = prevy + k * s0.dy;
                pt_vector vec =
                    path_to_poly45(prevx, prevy, newx, newy, widths[idx] / 2, sty0, sty1);
                tmp.set(vec.begin(), vec.end());
                ans.insert(tmp);
                traits::pt_list<pt_vector>::set_x(prev_pts, idx, newx);
                traits::pt_list<pt_vector>::set_y(prev_pts, idx, newy);
            }
        } else {
            for (std::size_t idx = 0; idx < n_paths; ++idx) {
                int32_t scale = deltas[idx];
                auto prevx = traits::pt_list<pt_vector>::x(prev_pts, idx);
                auto prevy = traits::pt_list<pt_vector>::y(prev_pts, idx);
                coord_t pdx = xc + dir1.dx * scale - prevx;
                coord_t pdy = yc + dir1.dy * scale - prevy;
                int32_t k = (pdx * s1.dy - pdy * s1.dx) / (s0.dx * s1.dy - s0.dy * s1.dx);
                coord_t newx = prevx + k * s0.dx;
                coord_t newy = prevy + k * s0.dy;
                pt_vector vec =
                    path_to_poly45(prevx, prevy, newx, newy, widths[idx] / 2, sty0, sty1);
                tmp.set(vec.begin(), vec.end());
                ans.insert(tmp);
                traits::pt_list<pt_vector>::set_x(prev_pts, idx, newx);
                traits::pt_list<pt_vector>::set_y(prev_pts, idx, newy);
            }
        }
    }

    // add last path segment
    auto sty0 = static_cast<end_style>((n_pts == 2) ? style0 : stylem);
    sty1 = static_cast<end_style>(style1);
    x0 = traits::pt_list<T>::x(data, n_pts - 1);
    y0 = traits::pt_list<T>::y(data, n_pts - 1);
    s0.dx = x0 - traits::pt_list<T>::x(data, n_pts - 2);
    s0.dy = y0 - traits::pt_list<T>::y(data, n_pts - 2);
    s0.rotate90_norm();
    if (s0.is_45_or_invalid()) {
        for (std::size_t idx = 0; idx < n_paths; ++idx) {
            int32_t scale = round(deltas[idx] / root2);
            auto xt = traits::pt_list<pt_vector>::x(prev_pts, idx);
            auto yt = traits::pt_list<pt_vector>::y(prev_pts, idx);
            pt_vector vec = path_to_poly45(xt, yt, x0 + s0.dx * scale, y0 + s0.dy * scale,
                                           widths[idx] / 2, sty0, sty1);
            tmp.set(vec.begin(), vec.end());
            ans.insert(tmp);
        }
    } else {
        for (std::size_t idx = 0; idx < n_paths; ++idx) {
            int32_t scale = deltas[idx];
            auto xt = traits::pt_list<pt_vector>::x(prev_pts, idx);
            auto yt = traits::pt_list<pt_vector>::y(prev_pts, idx);
            pt_vector vec = path_to_poly45(xt, yt, x0 + s0.dx * scale, y0 + s0.dy * scale,
                                           widths[idx] / 2, sty0, sty1);
            tmp.set(vec.begin(), vec.end());
            ans.insert(tmp);
        }
    }

    return ans;
}

template <typename T, typename = IsPtList<T>>
shape_ref<polygon45_set> add_path(cellview &cv, const std::string &layer,
                                  const std::string &purpose, bool is_horiz, const T &data,
                                  offset_t half_width, uint8_t style0, uint8_t style1,
                                  uint8_t stylem, bool commit) {
    return cv.add_poly45_set(layer, purpose, is_horiz,
                             make_path(data, half_width, style0, style1, stylem), commit);
}

template <typename T, typename L, typename = IsPtList<T>>
shape_ref<polygon45_set> add_path45_bus(cellview &cv, const std::string &layer,
                                        const std::string &purpose, bool is_horiz, const T &data,
                                        const L &widths, const L &spaces, uint8_t style0,
                                        uint8_t style1, uint8_t stylem, bool commit) {
    return cv.add_poly45_set(layer, purpose, is_horiz,
                             make_path45_bus(data, widths, spaces, style0, style1, stylem), commit);
}

} // namespace layout
} // namespace cbag

#endif
