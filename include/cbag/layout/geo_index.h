#ifndef CBAG_LAYOUT_GEO_INDEX_H
#define CBAG_LAYOUT_GEO_INDEX_H

#include <cbag/common/layer_t.h>
#include <cbag/common/transformation_fwd.h>
#include <cbag/layout/geo_index_impl.h>
#include <cbag/layout/polygon45_set_fwd.h>

namespace cbag {

class box_t;

namespace layout {

class tech;
class geo_iterator;

class geo_index {
  public:
    using const_iterator = geo_iterator;

  private:
    geo_index_impl index;

  public:
    geo_index();

    bool empty() const;

    box_t get_bbox() const;

    const_iterator begin_intersect(const box_t &r, offset_t spx, offset_t spy,
                                   const cbag::transformation &xform) const;

    void insert(const std::shared_ptr<const geo_index> &master, const cbag::transformation &xform);

    template <typename T> void insert(T &&obj, offset_t spx, offset_t spy) {
        index.insert(geo_object(std::forward<T>(obj), spx, spy));
    }
};

} // namespace layout
} // namespace cbag

#endif
