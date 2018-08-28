/** \file yaml_datatypes.h
 *  \brief This file declares YAML serialization methods for datatypes.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_DATATYPES_H
#define CBAG_YAML_DATATYPES_H

#include <boost/fusion/include/adapt_struct.hpp>

#include <yaml-cpp/yaml.h>

#include <cbag/common/datatypes.h>

#include <cbagyaml/fusion.h>

BOOST_FUSION_ADAPT_STRUCT(cbag::time_struct, time_val)

BOOST_FUSION_ADAPT_STRUCT(cbag::binary_t, name, bin_val)

namespace YAML {
template <> struct convert<cbag::value_t> {
    static Node encode(const cbag::value_t &rhs);

    static bool decode(const Node &node, cbag::value_t &rhs);
};
} // namespace YAML

#endif // CBAG_YAML_DATATYPES_H
