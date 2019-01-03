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
    layer_t lay_purp;
    const tech *tech_ptr = nullptr;

  public:
    geo_index();

    geo_index(layer_t &&lay_purp, const tech *tech_ptr);

    bool empty() const;

    box_t get_bbox() const;

    const_iterator begin_intersect(const box_t &r, offset_t spx, offset_t spy,
                                   const cbag::transformation &xform) const;

    void insert(const box_t &obj, bool is_horiz);
    void insert(const polygon90 &obj, bool is_horiz);
    void insert(const polygon45 &obj, bool is_horiz);
    void insert(const polygon &obj, bool is_horiz);
    void insert(const polygon45_set &obj, bool is_horiz);
    void insert(const geometry *master, cbag::transformation &&xform);
};

} // namespace layout
} // namespace cbag

#endif
