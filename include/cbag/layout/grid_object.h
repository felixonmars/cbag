#ifndef CBAG_LAYOUT_GRID_OBJECT_H
#define CBAG_LAYOUT_GRID_OBJECT_H

#include <array>
#include <memory>
#include <string>
#include <tuple>

#include <cbag/common/box_t.h>
#include <cbag/common/layer_t.h>
#include <cbag/common/typedefs.h>
#include <cbag/enum/direction.h>
#include <cbag/layout/wire_width.h>

namespace cbag {
namespace layout {

class routing_grid;

class track_id : public std::enable_shared_from_this<track_id> {
  private:
    level_t level = 0;
    htr_t htr = 0;
    cnt_t ntr = 1;
    cnt_t num = 1;
    htr_t pitch = 0;

  public:
    track_id();

    track_id(level_t level, htr_t htr, cnt_t ntr, cnt_t num, htr_t pitch);

    bool operator==(const track_id &rhs) const noexcept;

    htr_t operator[](std::size_t idx) const noexcept;

    std::size_t get_hash() const noexcept;

    std::string to_string() const noexcept;

    level_t get_level() const noexcept;

    htr_t get_htr() const noexcept;

    cnt_t get_ntr() const noexcept;

    cnt_t get_num() const noexcept;

    htr_t get_pitch() const noexcept;

    void set_htr(htr_t val) noexcept;

    void set_pitch(htr_t val) noexcept;
};

class warr_rect_iterator;

class wire_array {
  private:
    std::shared_ptr<track_id> tid_ptr;
    std::array<offset_t, 2> coord;

  public:
    wire_array();

    wire_array(std::shared_ptr<track_id> tid, offset_t lower, offset_t upper);

    bool operator==(const wire_array &rhs) const noexcept;

    std::size_t get_hash() const;

    std::string to_string() const;

    const std::array<offset_t, 2> &get_coord() const noexcept;

    offset_t get_coord(direction dir) const noexcept;

    std::shared_ptr<track_id> get_track_id() const noexcept;

    const track_id &get_track_id_ref() const;

    offset_t get_middle() const noexcept;

    void set_track_id(std::shared_ptr<track_id> tid) noexcept;

    void set_coord(offset_t lower, offset_t upper) noexcept;

    warr_rect_iterator begin_rect(const routing_grid &grid) const;
    warr_rect_iterator end_rect(const routing_grid &grid) const;
};

class warr_rect_iterator {
  private:
    const routing_grid *grid_ptr;
    const wire_array *warr_ptr;
    cnt_t warr_idx = 0;
    cnt_t ww_idx = 0;
    wire_width wire_w;

  public:
    warr_rect_iterator();

    warr_rect_iterator(const routing_grid &grid, const wire_array &warr, bool begin);

    bool operator==(const warr_rect_iterator &rhs) const;
    bool operator!=(const warr_rect_iterator &rhs) const;

    std::tuple<layer_t, box_t> operator*() const;
    warr_rect_iterator &operator++();

    const routing_grid &get_grid() const;
};

} // namespace layout
} // namespace cbag

#endif
