/** \file yaml_primitives.h
 *  \brief This file declares YAML serialization methods for primitive objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_DATABASE_YAML_PRIMITIVES_H
#define CBAG_DATABASE_YAML_PRIMITIVES_H

#include <yaml-cpp/yaml.h>

#include <cbag/database/primitives.h>

namespace YAML {
    template<>
    struct convert<cbag::Point> {
        static Node encode(const cbag::Point &rhs);

        static bool decode(const Node &node, cbag::Point &rhs);
    };

    template<>
    struct convert<cbag::Transform> {
        static Node encode(const cbag::Transform &rhs);

        static bool decode(const Node &node, cbag::Transform &rhs);
    };

    template<>
    struct convert<cbag::BBox> {
        static Node encode(const cbag::BBox &rhs);

        static bool decode(const Node &node, cbag::BBox &rhs);
    };

    template<>
    struct convert<cbag::PointArray> {
        static Node encode(const cbag::PointArray &rhs);

        static bool decode(const Node &node, cbag::PointArray &rhs);
    };
}


#endif //CBAG_DATABASE_YAML_PRIMITIVES_H
