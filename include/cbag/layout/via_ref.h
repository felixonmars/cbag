#ifndef CBAG_LAYOUT_VIA_REF_H
#define CBAG_LAYOUT_VIA_REF_H

#include <vector>

#include <cbag/layout/via.h>

namespace cbag {
namespace layout {

class via_ref {
  private:
    std::vector<via> *vec = nullptr;
    std::size_t idx = 0;

  public:
    via_ref() = default;

    via_ref(std::vector<via> *vec, std::size_t idx) : vec(vec), idx(idx) {}

    via &value() { return (*vec)[idx]; }
};

} // namespace layout
} // namespace cbag

#endif
