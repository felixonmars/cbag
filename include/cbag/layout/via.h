#ifndef CBAG_LAYOUT_VIA_H
#define CBAG_LAYOUT_VIA_H

#include <string>

#include <cbag/common/typedefs.h>
#include <cbag/common/via_param.h>
#include <cbag/layout/polygon_ref.h>
#include <cbag/layout/transformation.h>

namespace cbag {
namespace layout {

class rectangle;

class via {
  private:
    polygon_ref<rectangle> lay1_ref;
    polygon_ref<rectangle> lay2_ref;
    struct helper;

  public:
    transformation xform;
    std::string via_id;
    via_param params;

  public:
    via(transformation xform, const char *via_id, const uint32_t (&num)[2],
        const dist_t (&cut_dim)[2], const offset_t (&cut_sp)[2], const offset_t (&lay1_enc)[2],
        const offset_t (&lay1_off)[2], const offset_t (&lay2_enc)[2],
        const offset_t (&lay2_off)[2]);

    rectangle bot_box() const;

    rectangle top_box() const;

    void set_layer1(polygon_ref<rectangle> ref);

    void set_layer2(polygon_ref<rectangle> ref);
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
