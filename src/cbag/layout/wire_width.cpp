#include <algorithm>

#include <cbag/layout/tech_util.h>
#include <cbag/layout/wire_width.h>

namespace cbag {
namespace layout {

wire_width::width_iter::width_iter() = default;

wire_width::width_iter::width_iter(vec_type::const_iterator iter) : vec_iter(iter) {}

auto wire_width::width_iter::operator++() -> width_iter & {
    ++vec_iter;
    return *this;
}

auto wire_width::width_iter::operator*() const -> reference { return std::get<1>(*vec_iter); }

bool wire_width::width_iter::operator==(const width_iter &rhs) const {
    return vec_iter == rhs.vec_iter;
}

bool wire_width::width_iter::operator!=(const width_iter &rhs) const {
    return vec_iter != rhs.vec_iter;
}

wire_width::wire_width(vec_type &&widths) : widths_(std::move(widths)) {}

auto wire_width::begin() const -> vec_type::const_iterator { return widths_.begin(); }

auto wire_width::end() const -> vec_type::const_iterator { return widths_.end(); }

auto wire_width::begin_width() const -> width_iter { return width_iter(widths_.begin()); }

auto wire_width::end_width() const -> width_iter { return width_iter(widths_.end()); }

offset_t wire_width::get_edge_wire_width() const { return std::get<1>(widths_[0]); }

offset_t wire_width::get_total_width(offset_t half_pitch) const {
    auto &[htr, w] = widths_[0];
    return w + half_pitch * (std::get<0>(widths_[widths_.size() - 1]) - htr);
}

} // namespace layout
} // namespace cbag
