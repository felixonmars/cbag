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

  public:
    label();

    label(layer_t key, transformation xform, std::string text);

    bool operator==(const label &rhs) const noexcept;
};

} // namespace layout
} // namespace cbag

#endif
