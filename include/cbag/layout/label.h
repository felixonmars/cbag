#ifndef CBAG_LAYOUT_LABEL_H
#define CBAG_LAYOUT_LABEL_H

#include <string>

#include <cbag/common/box_t.h>
#include <cbag/common/layer_t.h>
#include <cbag/common/transformation.h>

namespace cbag {
namespace layout {

class label {
  private:
    layer_t lay_purp;
    transformation xform;
    std::string text;
    offset_t height;

  public:
    label();

    label(layer_t key, transformation xform, std::string text, offset_t height);

    layer_t get_key() const;
    transformation get_xform() const;
    const std::string &get_text() const;
    offset_t get_height() const noexcept;

    bool operator==(const label &rhs) const noexcept;
};

} // namespace layout
} // namespace cbag

#endif
