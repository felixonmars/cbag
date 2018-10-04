#ifndef CBAG_LAYOUT_GEO_INDEX_H
#define CBAG_LAYOUT_GEO_INDEX_H

#include <cbag/layout/geo_index_impl.h>
#include <cbag/layout/polygon45_set_fwd.h>

namespace cbag {
namespace layout {

class tech;
class geo_iterator;

class geo_index {
  public:
    using const_iterator = geo_iterator;

  private:
    geo_index_impl index;
    std::string lay_type = "";
    tech *tech_ptr = nullptr;

  public:
    geo_index();

    explicit geo_index(std::string &&lay_type, tech *tech_ptr);

    const_iterator qbegin(rectangle box, offset_t spx, offset_t spy) const;
    const_iterator qend() const;

    void insert(const rectangle &obj, bool is_horiz);
    void insert(const polygon90 &obj, bool is_horiz);
    void insert(const polygon45 &obj, bool is_horiz);
    void insert(const polygon &obj, bool is_horiz);
    void insert(const polygon45_set &obj, bool is_horiz);
    void insert(const geometry *master, transformation &&xform);
};

} // namespace layout
} // namespace cbag

#endif
