#include <cbag/common/box_t.h>
#include <cbag/common/box_t_adapt.h>
#include <cbag/common/transformation.h>
#include <cbag/layout/geo_iterator.h>
#include <cbag/layout/geometry.h>

namespace cbag {
namespace layout {

struct geometry::helper {};

geometry::geometry() = default;

geometry::geometry(std::string &&lay_type, tech *tech_ptr, geometry_mode mode)
    : mode(mode), index(std::move(lay_type), tech_ptr) {
    if (mode != geometry_mode::POLY90)
        reset_to_mode(mode);
}

bool geometry::index_empty() const { return index.empty(); }

box_t geometry::get_bbox() const {
    box_t ans;
    bool success = std::visit(
        overload{
            [&ans](const auto &d) { return bp::extents(ans, d); },
        },
        data);

    if (!success)
        return box_t::get_invalid_box();
    return ans;
}

box_t geometry::get_index_bbox() const { return index.get_bbox(); }

geo_iterator geometry::begin_intersect(const box_t &r, offset_t spx, offset_t spy,
                                       const transformation &xform) const {
    return index.begin_intersect(r, spx, spy, xform);
}

void geometry::reset_to_mode(geometry_mode m) {
    switch (m) {
    case geometry_mode::POLY90:
        data.emplace<polygon90_set>();
        break;
    case geometry_mode::POLY45:
        data.emplace<polygon45_set>();
        break;
    case geometry_mode::POLY:
        data.emplace<polygon_set>();
        break;
    default:
        throw std::invalid_argument("Unknown geometry mode: " +
                                    std::to_string(static_cast<enum_t>(m)));
    }
    mode = m;
}

void geometry::add_shape(const box_t &obj, bool is_horiz) {
    std::visit(
        overload{
            [&obj](polygon90_set &d) { d.insert(obj); },
            [&obj](polygon45_set &d) { d.insert(obj); },
            [&obj](polygon_set &d) { d.insert(obj); },
        },
        data);

    index.insert(obj, is_horiz);
}

void geometry::add_shape(const polygon90 &obj, bool is_horiz) {
    std::visit(
        overload{
            [&obj](polygon90_set &d) { d.insert(obj); },
            [&obj](polygon45_set &d) { d.insert(obj); },
            [&obj](polygon_set &d) { d.insert(obj); },
        },
        data);

    index.insert(obj, is_horiz);
}

void geometry::add_shape(const polygon45 &obj, bool is_horiz) {
    std::visit(
        overload{
            [](polygon90_set &d) {
                throw std::invalid_argument("Cannot add poly45; incorrect cellview layout mode.");
            },
            [&obj](polygon45_set &d) { d.insert(obj); },
            [&obj](polygon_set &d) { d.insert(obj); },
        },
        data);

    index.insert(obj, is_horiz);
}

void geometry::add_shape(const polygon45_set &obj, bool is_horiz) {
    std::visit(
        overload{
            [](polygon90_set &d) {
                throw std::invalid_argument("Cannot add poly45; incorrect cellview layout mode.");
            },
            [&obj](polygon45_set &d) { d.insert(obj); },
            [&obj](polygon_set &d) { d.insert(obj); },
        },
        data);

    index.insert(obj, is_horiz);
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
            [&obj](polygon_set &d) { d.insert(obj); },
        },
        data);

    index.insert(obj, is_horiz);
}

void geometry::record_instance(const geometry *master, transformation xform) {
    index.insert(master, std::move(xform));
}

} // namespace layout
} // namespace cbag
