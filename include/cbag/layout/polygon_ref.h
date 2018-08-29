#ifndef CBAG_LAYOUT_POLYGON_REF_H
#define CBAG_LAYOUT_POLYGON_REF_H

#include <vector>

namespace cbag {
namespace layout {

template <typename T> class polygon_ref {
  private:
    std::vector<T> *vec = nullptr;
    std::size_t idx = 0;

  public:
    inline polygon_ref() {}

    inline polygon_ref(std::vector<T> *vec, std::size_t idx) : vec(vec), idx(idx) {}

    inline T &value() { return (*vec)[idx]; }
};

} // namespace layout
} // namespace cbag

#endif
