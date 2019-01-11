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
    int bot_level = 0;
    std::vector<track_info> info_list;
    int top_ignore_level = std::numeric_limits<int>::min();
    int top_private_level = std::numeric_limits<int>::min();
    struct helper;

  public:
    routing_grid();

    routing_grid(const tech *t, const std::string &fname);

    bool operator==(const routing_grid &rhs) const noexcept;

    const tech *get_tech() const noexcept;

    int get_bot_level() const noexcept;

    int get_top_ignore_level() const noexcept;

    int get_top_private_level() const noexcept;

    const track_info &get_track_info(int level) const;

    flip_parity get_flip_parity_at(int bot_level, int top_level, const transformation &xform) const;

    void set_flip_parity(const flip_parity &fp);
};

} // namespace layout
} // namespace cbag

#endif