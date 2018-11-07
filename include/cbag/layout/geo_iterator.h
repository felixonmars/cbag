#ifndef CBAG_LAYOUT_GEO_ITERATOR_H
#define CBAG_LAYOUT_GEO_ITERATOR_H

#include <memory>
#include <utility>

#include <cbag/common/box_t.h>
#include <cbag/layout/geo_index_impl.h>

namespace cbag {
namespace layout {

enum geo_union_enum : uint8_t {
    RECT = 0,
    POLY90 = 1,
    POLY45 = 2,
    POLY = 3,
};

class geo_union {
  public:
    std::variant<box_t, polygon90, polygon45, polygon> val;

    geo_union() = default;

    template <typename T> explicit geo_union(T &&t) : val(std::move(t)) {}

    template <typename T> explicit geo_union(const T &t) : val(t) {}

    template <typename T> T *get_if() { return std::get_if<T>(&val); }

    geo_union_enum index() const { return static_cast<geo_union_enum>(val.index()); }
};

class geo_iterator {
  public:
    using flat_geo_type = geo_union;
    using iterator_category = std::input_iterator_tag;
    using value_type = const flat_geo_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

  private:
    box_t box;
    offset_t spx = 0;
    offset_t spy = 0;
    geo_query_iter cur;
    geo_query_iter end;
    cbag::transformation xform;
    std::shared_ptr<geo_iterator> inner = nullptr;
    flat_geo_type cur_val;
    struct helper;

  public:
    geo_iterator();

    geo_iterator(const box_t &box, offset_t spx, offset_t spy, geo_query_iter &&cur,
                 geo_query_iter &&end, const cbag::transformation &xform);

    bool has_next() const;

    geo_iterator &operator++();
    geo_iterator operator++(int);
    reference operator*() const;
    bool operator==(const geo_iterator &rhs) const;
    bool operator!=(const geo_iterator &rhs) const;
};

} // namespace layout
} // namespace cbag

#endif
