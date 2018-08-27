#ifndef CBAG_LAYOUT_BLOCKAGE_H
#define CBAG_LAYOUT_BLOCKAGE_H

#include <cbag/common/blockage_type.h>
#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

class Blockage : public polygon {
  public:
    explicit inline Blockage(point_vector_t data, blockage_type type)
        : polygon(std::move(data)), type(type) {}

  private:
    blockage_type type;
};

} // namespace layout
} // namespace cbag

#endif
