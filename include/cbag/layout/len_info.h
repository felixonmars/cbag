
#ifndef CBAG_COMMON_LEN_INFO_H
#define CBAG_COMMON_LEN_INFO_H

#include <array>
#include <vector>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

using max_len_data = std::array<offset_t, 3>;

class len_info {
  private:
    std::vector<max_len_data> w_al_list;
    std::vector<max_len_data> md_al_list;

  public:
    len_info();

    len_info(std::vector<max_len_data> &&w_al, std::vector<max_len_data> &&md_al);

    offset_t get_min_length(offset_t w, bool even) const;
};

} // namespace layout
} // namespace cbag

#endif
