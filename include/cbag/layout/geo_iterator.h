#ifndef CBAG_LAYOUT_GEO_ITERATOR_H
#define CBAG_LAYOUT_GEO_ITERATOR_H

#include <memory>

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
    geo_query_iter ptr;
    std::shared_ptr<geo_iterator> start = nullptr;
    std::shared_ptr<geo_iterator> stop = nullptr;

  public:
    geo_iterator();

    geo_iterator(geo_query_iter &&cur, bool step);

    geo_iterator &operator++();
    geo_iterator operator++(int);
    value_type operator*() const;
    bool operator==(const geo_iterator &rhs) const;
    bool operator!=(const geo_iterator &rhs) const;
};

} // namespace layout
} // namespace cbag

#endif
