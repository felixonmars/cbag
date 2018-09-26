#ifndef CBAG_LAYOUT_BLOCKAGE_H
#define CBAG_LAYOUT_BLOCKAGE_H

#include <cbag/common/blockage_type.h>
#include <cbag/layout/typedefs.h>

namespace cbag {
namespace layout {

class blockage : public polygon {
  public:
    blockage_type type = blkRoute;
    lay_t layer;

    blockage(const pt_vector &data, blockage_type type, lay_t layer);
};

} // namespace layout
} // namespace cbag

#endif
