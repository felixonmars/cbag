#include <cbag/layout/len_info.h>

namespace cbag {
namespace layout {

len_info::len_info() = default;

len_info::len_info(std::vector<max_len_data> &&w_al, std::vector<max_len_data> &&md_al)
    : w_al_list(std::move(w_al)), md_al_list(std::move(md_al)) {}

offset_t len_info::get_min_length(offset_t w) const {
    // TODO: implement this
    return 0;
}

} // namespace layout
} // namespace cbag
