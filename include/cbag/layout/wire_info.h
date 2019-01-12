#ifndef CBAG_LAYOUT_WIRE_INFO_H
#define CBAG_LAYOUT_WIRE_INFO_H

#include <string>
#include <tuple>
#include <vector>

#include <cbag/common/layer_t.h>
#include <cbag/common/typedefs.h>
#include <cbag/enum/space_type.h>

namespace cbag {
namespace layout {

class tech;

/** A class to model a "conceptual wire".
 * A "conceptual wire" consists of one or more wires, since some technologies
 * have discrete width choices, so wide wires have to be realized with multiple
 * small wires.  This class encapsulates these technology details.
 *
 * Assumptions made by this class:
 * - minimum space DRC rules is the same for different color wires.
 * - all the individual wires are on track, and they are symmetric with respect
 *   to the center.
 */
class wire_info {
  private:
    std::vector<std::tuple<int, offset_t>> widths_;

  public:
    explicit wire_info(std::vector<std::tuple<int, offset_t>> &&widths);

    offset_t get_min_length(const tech &t, int_t level, bool even) const;

    offset_t get_min_space(const tech &t, int_t level, space_type sp_type, bool even) const;

    offset_t get_edge_wire_width() const;

    offset_t get_total_width(offset_t pitch) const;

    em_specs_t get_metal_em_specs(const tech &t, layer_t key, offset_t length, bool vertical,
                                  int_t dc_temp, int_t rms_dt) const;
};

} // namespace layout
} // namespace cbag

#endif
