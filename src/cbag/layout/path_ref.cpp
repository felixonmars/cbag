#include <cbag/layout/path_ref.h>
#include <cbag/layout/polygon45.h>

namespace cbag {
namespace layout {

path_ref::path_ref() = default;

path_ref::path_ref(std::vector<polygon45> *vec, std::size_t start, std::size_t stop)
    : vec(vec), start(start), stop(stop) {}

} // namespace layout
} // namespace cbag
