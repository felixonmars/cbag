/** \file yaml_figures.h
 *  \brief This file declares YAML serialization methods for shapes.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_DATABASE_YAML_SHAPES_H
#define CBAG_DATABASE_YAML_SHAPES_H

#include <yaml-cpp/yaml.h>

#include <cbag/yaml_fusion.h>
#include <cbag/database/yaml_primitives.h>
#include <cbag/database/shapes_adapted.h>


namespace YAML {
    template<>
    struct convert<cbag::Shape> {
        static Node encode(const cbag::Shape &rhs);

        static bool decode(const Node &node, cbag::Shape &rhs);
    };
}


#endif //CBAG_DATABASE_YAML_FIGURES_H
