#include <cbag/common/box_t.h>
#include <cbag/common/box_t_adapt.h>
#include <cbag/common/transformation.h>
#include <cbag/layout/geo_iterator.h>
#include <cbag/layout/geometry.h>
#include <cbag/layout/routing_grid_util.h>

namespace cbag {
namespace layout {

class poly45_writer {
  public:
    using value_type = polygon45;

  private:
    geo_index &index;
    const routing_grid &grid;
    layer_t key;
    value_type last;

  public:
    poly45_writer(geo_index &index, const routing_grid &grid, layer_t key)
        : index(index), grid(grid), key(std::move(key)) {}

    void push_back(value_type &&v) {
        record_last();
        last = std::move(v);
    }

    void insert(value_type *ptr, const value_type &v) {
        record_last();
        last = v;
    }

    void record_last() const {
        auto [spx, spy] = get_margins(grid, key, last);
        index.insert(last, spx, spy);
    }

    value_type &back() { return last; }

    value_type *end() const { return nullptr; }
};

struct geometry::helper {};

geometry::geometry(geometry_mode mode) : mode(mode) {
    if (mode != geometry_mode::POLY90)
        reset_to_mode(mode);
}

bool geometry::index_empty() const { return index.empty(); }

bool geometry::operator==(const geometry &rhs) const noexcept {
    return mode == rhs.mode && data == rhs.data;
}

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

void geometry::add_shape(const box_t &obj, offset_t spx, offset_t spy) {
    std::visit(
        overload{
            [&obj](polygon90_set &d) { d.insert(obj); },
            [&obj](polygon45_set &d) { d.insert(obj); },
            [&obj](polygon_set &d) { d.insert(obj); },
        },
        data);

    index.insert(obj, spx, spy);
}

void geometry::add_shape(const polygon90 &obj, offset_t spx, offset_t spy) {
    std::visit(
        overload{
            [&obj](polygon90_set &d) { d.insert(obj); },
            [&obj](polygon45_set &d) { d.insert(obj); },
            [&obj](polygon_set &d) { d.insert(obj); },
        },
        data);

    index.insert(obj, spx, spy);
}

void geometry::add_shape(const polygon45 &obj, offset_t spx, offset_t spy) {
    std::visit(
        overload{
            [](polygon90_set &d) {
                throw std::invalid_argument("Cannot add poly45; incorrect cellview layout mode.");
            },
            [&obj](polygon45_set &d) { d.insert(obj); },
            [&obj](polygon_set &d) { d.insert(obj); },
        },
        data);

    index.insert(obj, spx, spy);
}

void geometry::add_shape(const polygon &obj, offset_t spx, offset_t spy) {
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

    index.insert(obj, spx, spy);
}

void geometry::add_shape_set(const routing_grid &grid, layer_t key, const polygon45_set &obj) {
    std::visit(
        overload{
            [](polygon90_set &d) {
                throw std::invalid_argument("Cannot add poly45; incorrect cellview layout mode.");
            },
            [&obj](polygon45_set &d) { d.insert(obj); },
            [&obj](polygon_set &d) { d.insert(obj); },
        },
        data);

    poly45_writer writer(index, grid, key);
    obj.get(writer);
    writer.record_last();
}

void geometry::record_instance(const geometry *master, transformation xform) {
    index.insert(master, std::move(xform));
}

} // namespace layout
} // namespace cbag
