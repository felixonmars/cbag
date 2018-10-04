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

    value_type &back() { return last; }

    void record_last() const { index.insert(last, is_horiz); }

    value_type *end() const { return nullptr; }

    void insert(value_type *ptr, const value_type &v) { last = v; }
};

geo_index::geo_index() = default;

geo_index::geo_index(std::string &&lay_type, tech *tech_ptr)
    : lay_type(std::move(lay_type)), tech_ptr(tech_ptr) {}

rectangle &geo_index::get_bbox(rectangle &r) const {
    auto box = index.bounds();
    r.xl = box.min_corner().get<0>();
    r.yl = box.min_corner().get<1>();
    r.xh = box.max_corner().get<0>();
    r.yh = box.max_corner().get<1>();
    return r;
}

geo_index::const_iterator geo_index::qbegin(rectangle box, offset_t spx, offset_t spy) const {
    // TODO: fix this
    return qend();
}

geo_index::const_iterator geo_index::qend() const {
    // TODO: fix this
    return {};
}

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
    geo_object item(geo_instance(master, std::move(xform)), 0, 0);
    if (bg::is_valid(item.bnd_box))
        index.insert(item);
}

} // namespace layout
} // namespace cbag
