#ifndef CBAG_LAYOUT_GRID_OBJECT_H
#define CBAG_LAYOUT_GRID_OBJECT_H

#include <array>

#include <cbag/common/transformation_fwd.h>
#include <cbag/common/typedefs.h>
#include <cbag/enum/direction.h>

namespace cbag {
namespace layout {

class routing_grid;
class wire_array;

class track_id {
  private:
    level_t level = 0;
    htr_t htr = 0;
    cnt_t ntr = 1;
    cnt_t num = 1;
    offset_t pitch = 0;

  public:
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

    track_id &transform(const transformation &xform, const routing_grid &grid);

    track_id get_transform(const transformation &xform, const routing_grid &grid) const;

    friend wire_array;
};

class wire_array {
  private:
    track_id tid;
    std::array<offset_t, 2> coord;

  public:
    class warr_iterator;

    wire_array();

    wire_array(track_id tid, offset_t lower, offset_t upper);

    bool operator==(const wire_array &rhs) const noexcept;

    std::size_t get_hash() const noexcept;

    std::string to_string() const noexcept;

    offset_t get_coord(direction dir) const noexcept;

    const track_id &get_track_id() const noexcept;

    offset_t get_middle() const noexcept;

    warr_iterator begin() const noexcept;

    warr_iterator end() const noexcept;

    wire_array &transform(const transformation &xform, const routing_grid &grid);

    wire_array get_transform(const transformation &xform, const routing_grid &grid) const;
};

class wire_array::warr_iterator {
  private:
    wire_array val;
    offset_t pitch;
    cnt_t idx;

  public:
    warr_iterator();

    warr_iterator(track_id &&tid, const std::array<offset_t, 2> &coord, offset_t pitch, cnt_t idx);

    bool operator==(const warr_iterator &rhs) const noexcept;
    bool operator!=(const warr_iterator &rhs) const noexcept;

    const wire_array &operator*() const noexcept;
    warr_iterator &operator++() noexcept;
};

} // namespace layout
} // namespace cbag

#endif
