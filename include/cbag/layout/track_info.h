#ifndef CBAG_LAYOUT_TRACK_INFO_H
#define CBAG_LAYOUT_TRACK_INFO_H

#include <array>
#include <vector>

#include <cbag/util/interval.h>

#include <cbag/common/typedefs.h>
#include <cbag/enum/orient_2d.h>

namespace cbag {
namespace layout {

class tech;
class routing_grid;
class wire_info;

class track_info {
  private:
    orient_2d dir = orient_2d::HORIZONTAL;
    offset_t w = 0;
    offset_t sp = 0;
    offset_t offset = 0;
    offset_t par_scale = 1;
    offset_t par_offset = 0;
    std::array<offset_t, 2> blk_pitch = {1, 1};
    cbag::util::disjoint_intvs<> w_intvs;

    friend class routing_grid;

  public:
    track_info();

    track_info(orient_2d tr_dir, offset_t tr_w, offset_t tr_sp,
               const std::vector<std::array<offset_t, 2>> &intv_list);

    bool operator==(const track_info &rhs) const noexcept;

    bool compatible(const track_info &rhs) const noexcept;

    orient_2d get_direction() const noexcept;

    offset_t get_pitch() const noexcept;

    offset_t get_blk_pitch(bool half_blk) const noexcept;

    offset_t get_offset() const noexcept;

    offset_t get_wire_span(cnt_t num_tr) const noexcept;

    wire_info get_wire_info(cnt_t num_tr) const;

    cnt_t space_to_htr(offset_t space) const noexcept;
};

} // namespace layout
} // namespace cbag

// YAML deserialization specialization
namespace YAML {

class Node;

template <typename T> struct convert;

template <> struct convert<cbag::layout::track_info> {
    static bool decode(const Node &node, cbag::layout::track_info &rhs);
};

} // namespace YAML

#endif
