#ifndef CBAG_LAYOUT_ROUTING_GRID_FWD_H
#define CBAG_LAYOUT_ROUTING_GRID_FWD_H

#include <memory>

#include <cbag/common/transformation_fwd.h>
#include <cbag/common/typedefs.h>
#include <cbag/enum/orient_2d.h>

namespace cbag {

namespace layout {

class tech;
class track_info;
class flip_parity;

class routing_grid {
  private:
    std::shared_ptr<const tech> tech_ptr = nullptr;
    level_t bot_level = 0;
    std::vector<track_info> info_list;
    level_t top_ignore_level = -1;
    level_t top_private_level = -1;
    struct helper;

  public:
    routing_grid();

    routing_grid(std::shared_ptr<const tech> t, const std::string &fname);

    bool operator==(const routing_grid &rhs) const noexcept;

    const track_info &track_info_at(level_t level) const;

    const track_info &operator[](level_t level) const;

    const tech *get_tech() const noexcept;

    level_t get_bot_level() const noexcept;

    level_t get_top_level() const noexcept;

    std::size_t get_num_levels() const noexcept;

    level_t get_top_ignore_level() const noexcept;

    level_t get_top_private_level() const noexcept;

    flip_parity get_flip_parity_at(level_t bot_level, level_t top_level,
                                   const transformation &xform) const;

    cnt_t get_htr_parity(level_t level, htr_t htr) const;

    void set_flip_parity(const flip_parity &fp);

    void set_top_ignore_level(level_t new_level);

    void set_level(level_t level, bool is_private, orient_2d dir, offset_t w, offset_t sp);

    void set_track_offset(level_t level, offset_t offset);
};

} // namespace layout
} // namespace cbag

#endif
