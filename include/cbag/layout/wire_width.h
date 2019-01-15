#ifndef CBAG_LAYOUT_WIRE_WIDTH_H
#define CBAG_LAYOUT_WIRE_WIDTH_H

#include <iterator>
#include <tuple>
#include <vector>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

class tech;

/** A class that represents the "width" of a "conceptual wire".
 * A "conceptual wire" consists of one or more wires, since some technologies
 * have discrete width choices, so wide wires have to be realized with multiple
 * small wires.  This class encapsulates the "width" detail of a wire.
 */
class wire_width {
  private:
    using vec_type = std::vector<std::tuple<htr_t, offset_t>>;
    vec_type widths_;

  public:
    class width_iter {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = offset_t;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type *;
        using reference = const value_type &;

      private:
        vec_type::const_iterator vec_iter;

      public:
        width_iter();

        explicit width_iter(vec_type::const_iterator iter);

        width_iter &operator++();
        reference operator*() const;
        bool operator==(const width_iter &rhs) const;
        bool operator!=(const width_iter &rhs) const;
    };

    explicit wire_width(vec_type &&widths);

    vec_type::const_iterator begin() const;

    vec_type::const_iterator end() const;

    width_iter begin_width() const;

    width_iter end_width() const;

    offset_t get_edge_wire_width() const;

    offset_t get_total_width(offset_t half_pitch) const;
};

} // namespace layout
} // namespace cbag

#endif
