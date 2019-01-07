#ifndef CBAG_LAYOUT_WIRE_INFO_H
#define CBAG_LAYOUT_WIRE_INFO_H

#include <string>
#include <tuple>
#include <vector>

#include <cbag/common/typedefs.h>
#include <cbag/enum/space_type.h>

namespace cbag {
namespace layout {

class tech;

class wire_info {
  private:
    std::vector<std::tuple<int, offset_t>> widths_;

  public:
    explicit wire_info(std::vector<std::tuple<int, offset_t>> &&widths);

    offset_t get_min_length(const tech &t, int level, bool even) const;

    offset_t get_min_space(const tech &t, int level, space_type sp_type, bool even) const;
};

} // namespace layout
} // namespace cbag

#endif
