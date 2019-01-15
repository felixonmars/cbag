#ifndef CBAG_LAYOUT_TRACK_ID_H
#define CBAG_LAYOUT_TRACK_ID_H

#include <iterator>
#include <tuple>
#include <vector>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

class track_id {
  private:
    level_t level = 0;
    htr_t htr = 0;
    cnt_t ntr = 1;
    cnt_t num = 1;
    offset_t pitch = 0;

    track_id();

    track_id(level_t level, htr_t htr, cnt_t ntr, cnt_t num, offset_t pitch);

    bool operator==(const track_id &rhs) const noexcept;

    std::size_t get_hash() const noexcept;

    std::string to_string() const noexcept;

    level_t get_level() const noexcept;

    htr_t get_htr() const noexcept;

    cnt_t get_ntr() const noexcept;

    cnt_t get_num() const noexcept;

    offset_t get_pitch() const noexcept;
};

} // namespace layout
} // namespace cbag

#endif
