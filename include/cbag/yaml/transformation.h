/** \file yaml_primitives.h
 *  \brief This file declares YAML serialization methods for primitive objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_TRANSFORMATION_H
#define CBAG_YAML_TRANSFORMATION_H

#include <yaml-cpp/yaml.h>

#include <cbag/common/transformation_fwd.h>

namespace YAML {

template <> struct convert<cbag::transformation> {
    static Node encode(const cbag::transformation &rhs);

    static bool decode(const Node &node, cbag::transformation &rhs);
};

} // namespace YAML

#endif
