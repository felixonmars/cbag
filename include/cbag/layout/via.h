#ifndef CBAG_LAYOUT_VIA_H
#define CBAG_LAYOUT_VIA_H

#include <string>

#include <cbag/common/transformation.h>
#include <cbag/layout/via_param.h>

namespace cbag {

class box_t;

namespace layout {

class via {
  private:
    std::string via_id;
    via_param params;
    struct helper;

  public:
    cbag::transformation xform;

  public:
    via();

    via(cbag::transformation xform, std::string via_id, via_param params);

    const std::string &get_via_id() const;

    const via_param &get_params() const;
};

} // namespace layout
} // namespace cbag

#endif
