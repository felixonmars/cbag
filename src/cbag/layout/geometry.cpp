#include <algorithm>
#include <cmath>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cbag/layout/end_style.h>
#include <cbag/layout/geo_instance.h>
#include <cbag/layout/geo_object.h>
#include <cbag/layout/geometry.h>
#include <cbag/layout/rectangle.h>
#include <cbag/layout/tech.h>
#include <cbag/layout/transformation.h>
#include <cbag/layout/vector45.h>
#include <cbag/math/constexpr.h>

namespace cbag {
namespace layout {

struct geometry::helper {};

class poly45_writer {
  public:
    using value_type = bp::polygon_45_data<coord_t>;

  private:
    geometry_index &index;
    offset_t spx;
    offset_t spy;
    value_type last;

  public:
    poly45_writer(geometry_index &index, offset_t spx, offset_t spy)
        : index(index), spx(spx), spy(spy) {}

    void push_back(value_type &&v) {
        record_last();
        last = std::move(v);
    }

    value_type &back() { return last; }

    void record_last() const { index.insert(geo_object(last, spx, spy)); }

    value_type *end() const { return nullptr; }

    void insert(value_type *ptr, const value_type &v) { last = v; }
};

geometry::geometry() = default;

geometry::geometry(std::string &&lay_type, tech *tech_ptr, uint8_t mode)
    : mode(mode), lay_type(std::move(lay_type)), tech_ptr(tech_ptr) {
    if (mode != 0)
        reset_to_mode(mode);
}

rectangle geometry::get_bbox() const {
    rectangle ans;
    return get_bbox(ans);
}

rectangle &geometry::get_bbox(rectangle &r) const {
    std::visit(
        overload{
            [&](const auto &d) { bp::extents(r, d); },
        },
        data);

    return r;
}

void geometry::reset_to_mode(uint8_t m) {
    switch (m) {
    case 0:
        data.emplace<0>();
        break;
    case 1:
        data.emplace<1>();
        break;
    case 2:
        data.emplace<2>();
        break;
    default:
        throw std::invalid_argument(fmt::format("Unknown geometry mode: {}", m));
    }
    mode = m;
}

void geometry::add_shape(const rectangle &obj, bool is_horiz) {
    std::visit(
        overload{
            [&](polygon90_set &d) { d.insert(obj); },
            [&](polygon45_set &d) { d.insert(obj); },
            [&](polygon_set &d) { d.insert(obj); },
        },
        data);

    coord_t spx, spy;
    if (lay_type.empty()) {
        spx = spy = 0;
    } else {
        if (is_horiz) {
            offset_t w = obj.h();
            spx = tech_ptr->get_min_space(lay_type, w, space_type::LINE_END);
            spy = tech_ptr->get_min_space(lay_type, w, space_type::DIFF_COLOR);
        } else {
            offset_t w = obj.w();
            spx = tech_ptr->get_min_space(lay_type, w, space_type::DIFF_COLOR);
            spy = tech_ptr->get_min_space(lay_type, w, space_type::LINE_END);
        }
    }

    index.insert(geo_object(obj, spx, spy));
}

void geometry::add_shape(const polygon90 &obj, bool is_horiz) {
    std::visit(
        overload{
            [&](polygon90_set &d) { d.insert(obj); },
            [&](polygon45_set &d) { d.insert(obj); },
            [&](polygon_set &d) { d.insert(obj); },
        },
        data);
    // TODO: space around polygons?
    index.insert(geo_object(obj, 0, 0));
}

void geometry::add_shape(const polygon45 &obj, bool is_horiz) {
    std::visit(
        overload{
            [](polygon90_set &d) {
                throw std::invalid_argument("Cannot add poly45; incorrect cellview layout mode.");
            },
            [&](polygon45_set &d) { d.insert(obj); },
            [&](polygon_set &d) { d.insert(obj); },
        },
        data);
    // TODO: space around polygons?
    index.insert(geo_object(obj, 0, 0));
}

void geometry::add_shape(const polygon45_set &obj, bool is_horiz) {
    std::visit(
        overload{
            [](polygon90_set &d) {
                throw std::invalid_argument("Cannot add poly45; incorrect cellview layout mode.");
            },
            [&](polygon45_set &d) { d.insert(obj); },
            [&](polygon_set &d) { d.insert(obj); },
        },
        data);
    // TODO: space around polygons?
    poly45_writer writer(index, 0, 0);
    obj.get(writer);
    writer.record_last();
}

void geometry::add_shape(const polygon &obj, bool is_horiz) {
    std::visit(
        overload{
            [](polygon90_set &d) {
                throw std::invalid_argument("Cannot add poly; incorrect cellview layout mode.");
            },
            [](polygon45_set &d) {
                throw std::invalid_argument("Cannot add poly; incorrect cellview layout mode.");
            },
            [&](polygon_set &d) { d.insert(obj); },
        },
        data);
    // TODO: space around polygons?
    index.insert(geo_object(obj, 0, 0));
}

void geometry::record_instance(const geometry *master, transformation xform) {
    index.insert(geo_object(geo_instance(master, std::move(xform)), 0, 0));
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
    vector45 p_norm{x1 - x0, y1 - y0};
    p_norm.normalize();

    // handle empty path
    if (half_width == 0 || (p_norm.dx == 0 && p_norm.dy == 0)) {
        return {};
    }
    // handle invalid path
    if (!p_norm.valid()) {
        throw std::invalid_argument(fmt::format("path segment vector {} not valid", p_norm));
    }

    bool is_45 = p_norm.is_45_or_invalid();

    // initialize point array, reserve space for worst case
    pt_vector ans;
    ans.reserve(8);

    vector45 n_norm = p_norm.get_rotate90();
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

polygon45_set geometry::make_path(const pt_vector &data, offset_t half_width, uint8_t style0,
                                  uint8_t style1, uint8_t stylem) {
    pt_vector::size_type n = data.size();
    if (n < 2) {
        throw std::invalid_argument(fmt::format("Cannot draw path with less than 2 points."));
    }

    auto s0 = static_cast<end_style>(style0);
    auto s1 = static_cast<end_style>(style1);
    auto sm = static_cast<end_style>(stylem);

    polygon45_set ans;
    polygon45 tmp;
    for (pt_vector::size_type istart = 0; istart < n - 1; ++istart) {
        end_style c0 = (istart == 0) ? s0 : sm;
        end_style c1 = (istart == n - 2) ? s1 : sm;
        pt_vector pt_vec = path_to_poly45(data[istart].x(), data[istart].y(), data[istart + 1].x(),
                                          data[istart + 1].y(), half_width, c0, c1);
        tmp.set(pt_vec.begin(), pt_vec.end());
        ans.insert(tmp);
    }

    return ans;
}

polygon45_set geometry::make_path45_bus(const pt_vector &data, const std::vector<offset_t> &widths,
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
    polygon45_set ans;
    polygon45 tmp;
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
                pt_vector vec =
                    path_to_poly45(prevx, prevy, newx, newy, widths[idx] / 2, sty0, sty1);
                tmp.set(vec.begin(), vec.end());
                ans.insert(tmp);
                prev_pts[idx].x(newx);
                prev_pts[idx].y(newy);
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
                pt_vector vec =
                    path_to_poly45(prevx, prevy, newx, newy, widths[idx] / 2, sty0, sty1);
                tmp.set(vec.begin(), vec.end());
                ans.insert(tmp);
                prev_pts[idx].x(newx);
                prev_pts[idx].y(newy);
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
            pt_vector vec = path_to_poly45(prev_pts[idx].x(), prev_pts[idx].y(), x0 + s0.dx * scale,
                                           y0 + s0.dy * scale, widths[idx] / 2, sty0, sty1);
            tmp.set(vec.begin(), vec.end());
            ans.insert(tmp);
        }
    } else {
        for (std::size_t idx = 0; idx < n_paths; ++idx) {
            int32_t scale = deltas[idx];
            pt_vector vec = path_to_poly45(prev_pts[idx].x(), prev_pts[idx].y(), x0 + s0.dx * scale,
                                           y0 + s0.dy * scale, widths[idx] / 2, sty0, sty1);
            tmp.set(vec.begin(), vec.end());
            ans.insert(tmp);
        }
    }

    return ans;
}

} // namespace layout
} // namespace cbag
