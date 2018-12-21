#ifndef CBAG_LAYOUT_BLOCKAGE_H
#define CBAG_LAYOUT_BLOCKAGE_H

#include <cbag/common/blockage_type.h>
#include <cbag/common/typedefs.h>
#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

class blockage : public polygon {
  public:
    blockage_type type;
    lay_t layer;

    explicit blockage(blockage_type type = blockage_type::routing, lay_t layer = 0);
};

} // namespace layout
} // namespace cbag

#endif
