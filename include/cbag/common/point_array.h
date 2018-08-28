
#ifndef CBAG_COMMON_POINT_ARRAY_H
#define CBAG_COMMON_POINT_ARRAY_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using point_array = oa::oaPointArray;

} // namespace cbag

#else

#include <vector>

#include <cbag/common/point.h>

namespace cbag {

class point_array {
  private:
    std::vector<point> points;

  public:
    explicit point_array(uint32_t size = 0) : points(size) {}

    uint32_t getNumElements() const { return static_cast<uint32_t>(points.size()); }

    void setSize(uint32_t new_size, int save_elements = 0) { points.resize(new_size); }

    const point &operator[](uint32_t index) const { return points[index]; }

    point &operator[](uint32_t index) { return points[index]; }
};

} // namespace cbag

#endif
#endif
