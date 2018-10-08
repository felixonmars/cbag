#include <cbag/layout/geo_index.h>
#include <cbag/layout/geo_iterator.h>
#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

class poly45_writer {
  public:
    using value_type = polygon45;

  private:
    geo_index &index;
    bool is_horiz;
    value_type last;

  public:
    poly45_writer(geo_index &index, bool is_horiz) : index(index), is_horiz(is_horiz) {}

    void push_back(value_type &&v) {
        record_last();
        last = std::move(v);
    }

    void insert(value_type *ptr, const value_type &v) {
        record_last();
        last = v;
    }

    void record_last() const { index.insert(last, is_horiz); }

    value_type &back() { return last; }

    value_type *end() const { return nullptr; }
};

geo_index::geo_index() = default;

geo_index::geo_index(std::string &&lay_type, tech *tech_ptr)
    : lay_type(std::move(lay_type)), tech_ptr(tech_ptr) {}

bool geo_index::empty() const { return index.empty(); }

rectangle &geo_index::get_bbox(rectangle &r) const {
    auto box = index.bounds();
    r.xl = box.min_corner().get<0>();
    r.yl = box.min_corner().get<1>();
    r.xh = box.max_corner().get<0>();
    r.yh = box.max_corner().get<1>();
    return r;
}

geo_iterator geo_index::begin_intersect(const rectangle &r, offset_t spx, offset_t spy,
                                        const transformation &xform) const {

    return {r,
            spx,
            spy,
            index.qbegin(bgi::intersects(
                bg_box(bg_point(r.xl - spx, r.yl - spy), bg_point(r.xh + spx, r.yh + spy)))),
            index.qend(),
            xform};
}

geo_iterator geo_index::end_intersect() const { return {}; }

void geo_index::insert(const rectangle &obj, bool is_horiz) {
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

void geo_index::insert(const polygon90 &obj, bool is_horiz) {
    // TODO: space around polygons?
    index.insert(geo_object(obj, 0, 0));
}

void geo_index::insert(const polygon45 &obj, bool is_horiz) {
    // TODO: space around polygons?
    index.insert(geo_object(obj, 0, 0));
}

void geo_index::insert(const polygon &obj, bool is_horiz) {
    // TODO: space around polygons?
    index.insert(geo_object(obj, 0, 0));
}

void geo_index::insert(const polygon45_set &obj, bool is_horiz) {
    poly45_writer writer(*this, is_horiz);
    obj.get(writer);
    writer.record_last();
}

void geo_index::insert(const geometry *master, transformation &&xform) {
    geo_instance inst(master, std::move(xform));
    if (!inst.empty())
        index.insert(geo_object(std::move(inst), 0, 0));
}

} // namespace layout
} // namespace cbag
