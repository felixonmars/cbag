/** \file yaml_primitives.h
 *  \brief This file declares YAML serialization methods for primitive objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_POINT_ARRAY_H
#define CBAG_YAML_POINT_ARRAY_H

#include <yaml-cpp/yaml.h>

#include <cbag/common/point_array.h>

namespace YAML {

template <> struct convert<cbag::point_array> {
    static Node encode(const cbag::point_array &rhs);

    static bool decode(const Node &node, cbag::point_array &rhs);
};

} // namespace YAML

#endif
