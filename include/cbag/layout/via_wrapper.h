#ifndef CBAG_LAYOUT_VIA_WRAPPER_H
#define CBAG_LAYOUT_VIA_WRAPPER_H

#include <cbag/layout/via.h>

namespace cbag {

namespace layout {

struct via_wrapper {
  public:
    via v;
    bool add_layers;

    via_wrapper();

    via_wrapper(via &&v, bool add_layers);
};

} // namespace layout
} // namespace cbag

#endif
