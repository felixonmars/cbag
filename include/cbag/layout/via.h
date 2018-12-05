#ifndef CBAG_LAYOUT_VIA_H
#define CBAG_LAYOUT_VIA_H

#include <string>

#include <cbag/common/transformation.h>
#include <cbag/common/via_param.h>

namespace cbag {

class box_t;

namespace layout {

class via {
  private:
    struct helper;

  public:
    cbag::transformation xform;
    std::string via_id;
    via_param params;
    bool add_layers;
    bool bot_horiz;
    bool top_horiz;

  public:
    via();

    via(cbag::transformation xform, std::string via_id, const uint32_t (&num)[2],
        const dist_t (&cut_dim)[2], const offset_t (&cut_sp)[2], const offset_t (&lay1_enc)[2],
        const offset_t (&lay1_off)[2], const offset_t (&lay2_enc)[2], const offset_t (&lay2_off)[2],
        bool add_layers, bool bot_horiz, bool top_horiz);

    box_t bot_box() const;

    box_t top_box() const;
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
