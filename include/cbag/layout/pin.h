#ifndef CBAG_LAYOUT_PIN_H
#define CBAG_LAYOUT_PIN_H

#include <string>

#include <cbag/common/box_t.h>

namespace cbag {
namespace layout {

class pin : public box_t {
  private:
    std::string net;
    std::string label;

  public:
    pin();
    pin(coord_t xl, coord_t yl, coord_t xh, coord_t yh, std::string net, std::string label);
    pin(box_t box, std::string net, std::string label);

    const std::string &get_net() const noexcept;

    const std::string &get_label() const noexcept;

    bool operator==(const pin &rhs) const noexcept;
};

} // namespace layout
} // namespace cbag

#endif
