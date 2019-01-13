#ifndef CBAG_LAYOUT_ROUTING_GRID_FWD_H
#define CBAG_LAYOUT_ROUTING_GRID_FWD_H

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
    const tech *tech_ptr = nullptr;
    level_t bot_level = 0;
    std::vector<track_info> info_list;
    level_t top_ignore_level = std::numeric_limits<level_t>::min();
    level_t top_private_level = std::numeric_limits<level_t>::min();
    struct helper;

  public:
    routing_grid();

    routing_grid(const tech *t, const std::string &fname);

    bool operator==(const routing_grid &rhs) const noexcept;

    const track_info &track_info_at(level_t level) const;

    const track_info &operator[](level_t level) const;

    const tech *get_tech() const noexcept;

    int get_bot_level() const noexcept;

    int get_top_ignore_level() const noexcept;

    int get_top_private_level() const noexcept;

    flip_parity get_flip_parity_at(level_t bot_level, level_t top_level,
                                   const transformation &xform) const;

    cnt_t get_htr_parity(level_t level, htr_t htr) const;

    void set_flip_parity(const flip_parity &fp);
};

} // namespace layout
} // namespace cbag

#endif
