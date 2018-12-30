#ifndef CBAG_YAML_VIA_INFO_H
#define CBAG_YAML_VIA_INFO_H

#include <yaml-cpp/yaml.h>

#include <cbag/layout/via_info.h>

namespace YAML {

template <> struct convert<cbag::layout::venc_data> {
    static bool decode(const Node &node, cbag::layout::venc_data &rhs);
};

template <> struct convert<cbag::layout::via_info> {
    static bool decode(const Node &node, cbag::layout::via_info &rhs);
};

} // namespace YAML

#endif
