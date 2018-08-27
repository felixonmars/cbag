/** \file yaml_primitives.h
 *  \brief This file declares YAML serialization methods for primitive objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_POINT_H
#define CBAG_YAML_POINT_H

#include <yaml-cpp/yaml.h>

#include <cbag/common/point.h>

namespace YAML {

template <> struct convert<cbag::point> {
    static Node encode(const cbag::point &rhs);

    static bool decode(const Node &node, cbag::point &rhs);
};

} // namespace YAML

#endif
