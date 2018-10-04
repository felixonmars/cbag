#ifndef CBAG_LAYOUT_GEO_ITERATOR_H
#define CBAG_LAYOUT_GEO_ITERATOR_H

#include <memory>
#include <utility>

#include <cbag/layout/geo_index_impl.h>

namespace cbag {
namespace layout {

class geo_iterator {
  public:
    using flat_geo_type = std::variant<rectangle, polygon90, polygon45, polygon>;
    using iterator_category = std::input_iterator_tag;
    using value_type = const flat_geo_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

  private:
    rectangle box;
    offset_t spx = 0;
    offset_t spy = 0;
    geo_query_iter cur;
    geo_query_iter end;
    std::shared_ptr<std::pair<geo_iterator, geo_iterator>> inner = nullptr;
    struct helper;

  public:
    geo_iterator();

    geo_iterator(rectangle box, offset_t spx, offset_t spy, geo_query_iter &&cur,
                 geo_query_iter &&end);

    geo_iterator &operator++();
    geo_iterator operator++(int);
    value_type operator*() const;
    bool operator==(const geo_iterator &rhs) const;
    bool operator!=(const geo_iterator &rhs) const;
};

} // namespace layout
} // namespace cbag

#endif
