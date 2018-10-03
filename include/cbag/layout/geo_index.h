#ifndef CBAG_LAYOUT_GEO_INDEX_H
#define CBAG_LAYOUT_GEO_INDEX_H

#include <vector>

#include <boost/geometry/index/rtree.hpp>

#include <cbag/layout/geo_object.h>
#include <cbag/layout/polygon45_set.h>

namespace bgi = boost::geometry::index;

namespace cbag {
namespace layout {

class tech;

class geo_index {
  private:
    using index_impl = bgi::rtree<geo_object, bgi::quadratic<32, 16>>;

    index_impl index;
    std::string lay_type = "";
    tech *tech_ptr = nullptr;

    class geo_iterator {};

  public:
    using iterator = geo_iterator;

    geo_index();

    explicit geo_index(std::string &&lay_type, tech *tech_ptr);

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
