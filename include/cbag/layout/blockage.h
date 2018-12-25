#ifndef CBAG_LAYOUT_BLOCKAGE_H
#define CBAG_LAYOUT_BLOCKAGE_H

#include <cbag/common/typedefs.h>
#include <cbag/enum/blockage_type.h>
#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

class blockage : public polygon {
  private:
    blockage_type type;
    lay_t layer;

  public:
    explicit blockage(blockage_type type = blockage_type::routing, lay_t layer = 0);

    blockage_type get_type() const;
    lay_t get_layer() const;
};

} // namespace layout
} // namespace cbag

#endif
