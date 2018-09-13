#ifndef CBAG_LAYOUT_PATH_REF_H
#define CBAG_LAYOUT_PATH_REF_H

#include <vector>

namespace cbag {
namespace layout {

class polygon45;

class path_ref {
  private:
    std::vector<polygon45> *vec = nullptr;
    std::size_t start = 0;
    std::size_t stop = 0;

  public:
    path_ref();

    path_ref(std::vector<polygon45> *vec, std::size_t start, std::size_t stop);
};

} // namespace layout
} // namespace cbag

#endif
