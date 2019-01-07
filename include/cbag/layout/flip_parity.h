#ifndef CBAG_LAYOUT_FLIP_PARITY_H
#define CBAG_LAYOUT_FLIP_PARITY_H

#include <string>
#include <tuple>
#include <vector>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

class routing_grid;

class flip_parity {
  private:
    std::vector<std::tuple<int, offset_t, offset_t>> data_;

  public:
    explicit flip_parity(std::vector<std::tuple<int, offset_t, offset_t>> &&data);

    bool operator==(const flip_parity &rhs) const noexcept;

    std::size_t get_hash() const noexcept;

    std::string to_string() const noexcept;

    friend class routing_grid;
};

} // namespace layout
} // namespace cbag

#endif
