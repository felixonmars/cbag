#ifndef CBAG_LAYOUT_VECTOR_OBJ_REF_H
#define CBAG_LAYOUT_VECTOR_OBJ_REF_H

#include <vector>

namespace cbag {
namespace layout {

template <typename T> class vector_obj_ref {
  private:
    std::vector<T> *vec = nullptr;
    std::size_t idx = 0;

  public:
    vector_obj_ref() {}

    vector_obj_ref(std::vector<T> *vec, std::size_t idx) : vec(vec), idx(idx) {}

    T &value() { return (*vec)[idx]; }
};

} // namespace layout
} // namespace cbag

#endif
