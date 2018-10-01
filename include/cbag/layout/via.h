#ifndef CBAG_LAYOUT_VIA_H
#define CBAG_LAYOUT_VIA_H

#include <string>

#include <cbag/common/via_param.h>
#include <cbag/layout/transformation.h>

namespace cbag {
namespace layout {

class rectangle;

class via {
  private:
    struct helper;

  public:
    transformation xform;
    std::string via_id;
    via_param params;
    bool add_layers;
    bool bot_horiz;
    bool top_horiz;

  public:
    via();

    via(transformation xform, const char *via_id, const uint32_t (&num)[2],
        const dist_t (&cut_dim)[2], const offset_t (&cut_sp)[2], const offset_t (&lay1_enc)[2],
        const offset_t (&lay1_off)[2], const offset_t (&lay2_enc)[2], const offset_t (&lay2_off)[2],
        bool add_layers, bool bot_horiz, bool top_horiz);

    rectangle bot_box() const;

    rectangle top_box() const;
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
