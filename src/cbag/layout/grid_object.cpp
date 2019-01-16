#include <boost/container_hash/hash.hpp>

#include <fmt/core.h>

#include <cbag/common/transformation_util.h>
#include <cbag/layout/grid_object.h>
#include <cbag/layout/routing_grid.h>
#include <cbag/layout/track_info_util.h>
#include <cbag/util/math.h>

namespace cbag {
namespace layout {

track_id::track_id() = default;

track_id::track_id(level_t level, htr_t htr, cnt_t ntr, cnt_t num, offset_t pitch)
    : level(level), htr(htr), ntr(ntr), num(num), pitch(pitch) {
    if (num == 1) {
        pitch = 0;
    }
}

bool track_id::operator==(const track_id &rhs) const noexcept {
    return level == rhs.level && htr == rhs.htr && ntr == rhs.ntr && num == rhs.num &&
           pitch == rhs.pitch;
}

std::size_t track_id::get_hash() const noexcept {
    auto seed = static_cast<std::size_t>(0);
    boost::hash_combine(seed, level);
    boost::hash_combine(seed, htr);
    boost::hash_combine(seed, ntr);
    boost::hash_combine(seed, num);
    boost::hash_combine(seed, pitch);
    return seed;
}

std::string track_id::to_string() const noexcept {
    return fmt::format("TrackID(layer={}, htr={}, wdth={}, num={}, pitch={})", level, htr, ntr, num,
                       pitch);
}

level_t track_id::get_level() const noexcept { return level; }

htr_t track_id::get_htr() const noexcept { return htr; }

cnt_t track_id::get_ntr() const noexcept { return ntr; }

cnt_t track_id::get_num() const noexcept { return num; }

offset_t track_id::get_pitch() const noexcept { return pitch; }

track_id &track_id::transform(const transformation &xform, const routing_grid &grid) {
    auto &tinfo = grid.track_info_at(level);
    htr = transform_htr(tinfo, htr, xform);
    pitch *= axis_scale(xform)[1 - to_int(tinfo.get_direction())];
    return *this;
}

track_id track_id::get_transform(const transformation &xform, const routing_grid &grid) const {
    return track_id(*this).transform(xform, grid);
}

wire_array::wire_array() = default;

wire_array::wire_array(track_id tid, offset_t lower, offset_t upper)
    : tid(std::move(tid)), coord({lower, upper}) {}

bool wire_array::operator==(const wire_array &rhs) const noexcept {
    return tid == rhs.tid && coord == rhs.coord;
}

std::size_t wire_array::get_hash() const noexcept {
    auto seed = static_cast<std::size_t>(0);
    boost::hash_combine(seed, tid.get_hash());
    boost::hash_combine(seed, coord[0]);
    boost::hash_combine(seed, coord[1]);
    return seed;
}

std::string wire_array::to_string() const noexcept {
    return fmt::format("WireArray({}, {}, {})", tid.to_string(), coord[0], coord[1]);
}

offset_t wire_array::get_coord(direction dir) const noexcept { return coord[to_int(dir)]; }

const track_id &wire_array::get_track_id() const noexcept { return tid; }

offset_t wire_array::get_middle() const noexcept { return util::floor2(coord[0] + coord[1]); }

wire_array &wire_array::transform(const transformation &xform, const routing_grid &grid) {
    tid.transform(xform, grid);
    auto &tinfo = grid.track_info_at(tid.get_level());
    auto dir = tinfo.get_direction();
    auto scale = axis_scale(xform)[to_int(dir)];
    auto delta = cbag::get_coord(xform, dir);
    auto tmp = coord[0];
    if (scale != 1) {
        coord[0] = -coord[1];
        coord[1] = -tmp;
    }
    coord[0] += delta;
    coord[1] += delta;
    return *this;
}

wire_array wire_array::get_transform(const transformation &xform, const routing_grid &grid) const {
    return wire_array(*this).transform(xform, grid);
}

} // namespace layout
} // namespace cbag
