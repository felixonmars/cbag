#ifndef CBAG_YAML_LEN_INFO_H
#define CBAG_YAML_LEN_INFO_H

#include <yaml-cpp/yaml.h>

#include <cbag/layout/len_info.h>

namespace YAML {

template <> struct convert<cbag::layout::len_info> {
    static bool decode(const Node &node, cbag::layout::len_info &rhs);
};

} // namespace YAML

#endif
