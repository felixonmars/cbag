#include <fmt/core.h>

#include <yaml-cpp/yaml.h>

#include <cbag/logging/logging.h>

#include <cbag/layout/track_info.h>
#include <cbag/layout/wire_width.h>
#include <cbag/util/math.h>
#include <cbag/yaml/common.h>
#include <cbag/yaml/int_array.h>

namespace cbag {
namespace layout {

track_info::track_info() = default;

track_info::track_info(orient_2d tr_dir, offset_t tr_w, offset_t tr_sp,
                       const std::vector<std::array<offset_t, 2>> &intv_list)
    : dir(tr_dir), w(tr_w), sp(tr_sp),
      offset((tr_w + tr_sp) / 2), blk_pitch{tr_w + tr_sp, (tr_w + tr_sp) / 2} {
    for (const auto &intv : intv_list) {
        bool success = w_intvs.emplace(false, false, intv);
        if (!success) {
            throw std::invalid_argument(fmt::format(
                "Overlapping routing grid width intervals : [{}, {})", intv[0], intv[1]));
        }
    }
}

bool track_info::operator==(const track_info &rhs) const noexcept {
    return dir == rhs.dir && w == rhs.w && sp == rhs.sp && offset == rhs.offset &&
           par_scale == rhs.par_scale && par_offset == rhs.par_offset &&
           blk_pitch == rhs.blk_pitch && w_intvs == rhs.w_intvs;
}

bool track_info::compatible(const track_info &rhs) const noexcept {
    return dir == rhs.dir && w == rhs.w && sp == rhs.sp && offset == rhs.offset;
}

orient_2d track_info::get_direction() const noexcept { return dir; }

offset_t track_info::get_pitch() const noexcept { return w + sp; }

offset_t track_info::get_blk_pitch(bool half_blk) const noexcept {
    return blk_pitch[static_cast<int>(half_blk)];
}

offset_t track_info::get_offset() const noexcept { return offset; }

offset_t track_info::get_wire_span(cnt_t num_tr) const noexcept {
    return w + static_cast<offset_t>(num_tr - 1) * get_pitch();
}
wire_width track_info::get_wire_width(cnt_t num_tr) const {
    auto wire_w = get_wire_span(num_tr);
    return wire_width{
        std::vector<std::tuple<int_t, offset_t>>({std::tuple<int_t, offset_t>{0, wire_w}})};
}

cnt_t track_info::space_to_htr(offset_t space) const noexcept {
    return util::ceil(std::max(space - sp, 0), get_pitch() / 2);
}

} // namespace layout
} // namespace cbag

namespace YAML {

template <>
struct convert<std::array<cbag::offset_t, 2>> : public convert_arr<cbag::offset_t, 2> {};

bool convert<cbag::layout::track_info>::decode(const Node &node, cbag::layout::track_info &rhs) {
    auto logger = cbag::get_cbag_logger();
    if (!node.IsSequence() || node.size() != 4) {
        logger->warn(
            "cbag::layout::track_info YAML decode: not a sequence or size != 4.  Node:\n{}",
            cbagyaml::node_to_str(node));
        return false;
    }
    try {
        auto dir_str = node[0].as<std::string>();
        auto width = node[1].as<cbag::offset_t>();
        auto space = node[2].as<cbag::offset_t>();
        auto intv_list = node[3].as<std::vector<std::array<cbag::offset_t, 2>>>();

        auto tr_dir = (dir_str == "x") ? cbag::orient_2d::HORIZONTAL : cbag::orient_2d::VERTICAL;
        rhs = cbag::layout::track_info(tr_dir, width, space, intv_list);

    } catch (...) {
        logger->warn("cbag::layout::track_info YAML decode exception.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    return true;
}

} // namespace YAML
