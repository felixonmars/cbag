#ifndef CBAG_LAYOUT_POLYGON_REF_H
#define CBAG_LAYOUT_POLYGON_REF_H

#include <vector>

namespace cbag {
namespace layout {

template <typename T> class polygon_ref {
  public:
    inline polygon_ref() : vec(nullptr), idx(0) {}

    inline polygon_ref(std::vector<T> *vec, std::size_t idx) : vec(vec), idx(idx) {}

    inline T &value() { return (*vec)[idx]; }

  private:
    std::vector<T> *vec;
    std::size_t idx;
};

} // namespace layout
} // namespace cbag

#endif
