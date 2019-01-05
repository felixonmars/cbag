
#include <algorithm>

#include <fmt/core.h>

#include <cbag/common/box_t_adapt.h>
#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation.h>
#include <cbag/util/math.h>

namespace bp = boost::polygon;

namespace cbag {

box_t invalid_box() noexcept { return {0, 0, -1, -1}; }

offset_t get_dim(const box_t &box, orient_2d orient) {
    auto orient_code = to_int(orient);
    return box.intvs[orient_code][1] - box.intvs[orient_code][0];
}
coord_t get_center(const box_t &box, orient_2d orient) {
    auto orient_code = to_int(orient);
    return cbag::util::floor2(box.intvs[orient_code][0] + box.intvs[orient_code][1]);
}

void set(box_t &box, coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
    box.intvs[0][0] = xl;
    box.intvs[0][1] = xh;
    box.intvs[1][0] = yl;
    box.intvs[1][1] = yh;
}
void set_interval(box_t &box, orient_2d orient, coord_t tl, coord_t th) {
    auto orient_code = to_int(orient);
    box.intvs[orient_code][0] = tl;
    box.intvs[orient_code][1] = th;
}

coord_t xl(const box_t &box) { return box.intvs[0][0]; }
coord_t yl(const box_t &box) { return box.intvs[1][0]; }
coord_t xh(const box_t &box) { return box.intvs[0][1]; }
coord_t yh(const box_t &box) { return box.intvs[1][1]; }
coord_t xm(const box_t &box) { return get_center(box, orient_2d::HORIZONTAL); }
coord_t ym(const box_t &box) { return get_center(box, orient_2d::VERTICAL); }
coord_t width(const box_t &box) { return get_dim(box, orient_2d::HORIZONTAL); }
coord_t height(const box_t &box) { return get_dim(box, orient_2d::VERTICAL); }

std::string to_string(const box_t &box) {
    return fmt::format("BBox({}, {}, {}, {})", xl(box), yl(box), xh(box), yh(box));
}
bool is_physical(const box_t &box) { return width(box) > 0 && height(box) > 0; }
bool is_valid(const box_t &box) { return width(box) >= 0 && height(box) >= 0; }
bool overlaps(const box_t &box, const box_t &other) {
    return is_valid(box) && is_physical(other) && bp::intersects(box, other, true);
}

box_t &merge(box_t &box, const box_t &other) {
    if (!is_valid(other))
        return box;
    if (!is_valid(box))
        box = other;
    bp::encompass(box, other);
    return box;
}
box_t get_merge(box_t box, const box_t &other) { return merge(box, other); }

box_t &intersect(box_t &box, const box_t &other) {
    set(box, std::max(xl(box), xl(other)), std::max(yl(box), yl(other)),
        std::min(xh(box), xh(other)), std::min(yh(box), yh(other)));
    return box;
}
box_t get_intersect(box_t box, const box_t &other) { return intersect(box, other); }

box_t &extend_orient(box_t &box, orient_2d orient, const std::optional<coord_t> &ct,
                     const std::optional<coord_t> &cp) {
    auto orient_code = to_int(orient);
    if (is_valid(box)) {
        if (ct) {
            box.intvs[orient_code][0] = std::min(box.intvs[orient_code][0], *ct);
            box.intvs[orient_code][1] = std::max(box.intvs[orient_code][1], *ct);
        }
        if (cp) {
            box.intvs[1 - orient_code][0] = std::min(box.intvs[1 - orient_code][0], *cp);
            box.intvs[1 - orient_code][1] = std::max(box.intvs[1 - orient_code][1], *cp);
        }
    }
    return box;
}
box_t get_extend_orient(box_t box, orient_2d orient, const std::optional<coord_t> &ct,
                        const std::optional<coord_t> &cp) {
    return extend_orient(box, orient, ct, cp);
}

box_t &extend(box_t &box, const std::optional<coord_t> &x, const std::optional<coord_t> &y) {
    return extend_orient(box, orient_2d::HORIZONTAL, x, y);
}
box_t get_extend(box_t box, const std::optional<coord_t> &x, const std::optional<coord_t> &y) {
    return extend_orient(box, orient_2d::HORIZONTAL, x, y);
}

box_t &expand(box_t &box, coord_t dx, coord_t dy) {
    if (is_valid(box)) {
        set(box, xl(box) - dx, yl(box) - dy, xh(box) + dx, yh(box) + dy);
    }
    return box;
}
box_t get_expand(box_t box, coord_t dx, coord_t dy) { return expand(box, dx, dy); }

box_t &transform(box_t &box, const transformation &xform) { return bp::transform(box, xform); }
box_t get_transform(box_t box, const transformation &xform) { return bp::transform(box, xform); }

box_t &move_by_orient(box_t &box, orient_2d orient, offset_t dt, offset_t dp) {
    auto orient_code = to_int(orient);
    box.intvs[orient_code][0] += dt;
    box.intvs[orient_code][1] += dt;
    box.intvs[1 - orient_code][0] += dp;
    box.intvs[1 - orient_code][1] += dp;
    return box;
}
box_t get_move_by_orient(box_t box, orient_2d orient, offset_t dt, offset_t dp) {
    return move_by_orient(box, orient, dt, dp);
}

box_t &move_by(box_t &box, offset_t dx, offset_t dy) {
    return move_by_orient(box, orient_2d::HORIZONTAL, dx, dy);
}
box_t get_move_by(box_t box, offset_t dx, offset_t dy) {
    return move_by_orient(box, orient_2d::HORIZONTAL, dx, dy);
}

box_t &flip_xy(box_t &box) {
    auto tmp = box.intvs[0];
    box.intvs[0] = box.intvs[1];
    box.intvs[1] = tmp;
    return box;
}
box_t get_flip_xy(box_t box) { return flip_xy(box); }

} // namespace cbag
