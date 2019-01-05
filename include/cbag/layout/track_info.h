#ifndef CBAG_LAYOUT_TRACK_INFO_H
#define CBAG_LAYOUT_TRACK_INFO_H

#include <array>
#include <vector>

#include <cbag/util/interval.h>

#include <cbag/common/typedefs.h>
#include <cbag/enum/orient_2d.h>

namespace bp = boost::polygon;

namespace cbag {
namespace layout {

class track_info {
  private:
    orient_2d dir = orient_2d::HORIZONTAL;
    offset_t w = 0;
    offset_t sp = 0;
    cbag::util::disjoint_intvs<> w_intvs;

  public:
    track_info();

    track_info(orient_2d tr_dir, offset_t tr_w, offset_t tr_sp,
               const std::vector<std::array<offset_t, 2>> &intv_list);

    bool operator==(const track_info &rhs) const noexcept;
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
