#ifndef CBAG_LAYOUT_BLOCKAGE_H
#define CBAG_LAYOUT_BLOCKAGE_H

#include <cbag/common/blockage_type.h>
#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

class pt_vector;

class blockage : public polygon {
  public:
    blockage_type type = blkRoute;

    blockage(pt_vector data, blockage_type type);
};

} // namespace layout
} // namespace cbag

#endif
