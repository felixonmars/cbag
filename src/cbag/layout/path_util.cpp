#include <fmt/core.h>
#include <fmt/ostream.h>

#include <cbag/layout/path_util.h>

namespace cbag {
namespace layout {

void add_path_points(pt_vector &vec, coord_t x, coord_t y, const vector45 &p, const vector45 &n,
                     bool is_45, end_style style, offset_t w_main, offset_t w_norm) {
    switch (style) {
    case end_style::truncate: {
        auto xw = n.dx * w_main;
        auto yw = n.dy * w_main;
        vec.emplace_back(x + xw, y + yw);
        vec.emplace_back(x - xw, y - yw);
        break;
    }
    case end_style::extend:
        vec.emplace_back(x - (p.dx - n.dx) * w_main, y - (p.dy - n.dy) * w_main);
        vec.emplace_back(x - (p.dx + n.dx) * w_main, y - (p.dy + n.dy) * w_main);
        break;
    case end_style::triangle: {
        auto xw = n.dx * w_main;
        auto yw = n.dy * w_main;
        vec.emplace_back(x + xw, y + yw);
        vec.emplace_back(x - w_main * p.dx, y - w_main * p.dy);
        vec.emplace_back(x - xw, y - yw);
        break;
    }
    default: {
        auto xnm = n.dx * w_main;
        auto ynm = n.dy * w_main;
        auto xpm = p.dx * w_main;
        auto ypm = p.dy * w_main;
        auto xnn = n.dx * w_norm;
        auto ynn = n.dy * w_norm;
        auto xpn = p.dx * w_norm;
        auto ypn = p.dy * w_norm;
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
    auto half_diag = round(half_width / root2);
    offset_t w_main, w_norm;
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
