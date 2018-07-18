/** \file yaml_figures.h
 *  \brief This file declares YAML serialization methods for figures.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_DATABASE_YAML_FIGURES_H
#define CBAG_DATABASE_YAML_FIGURES_H

#include <cbag/database/yaml_primitives.h>
#include <cbag/database/yaml_datatypes.h>
#include <cbag/database/figures_adapted.h>


namespace YAML {
    template<>
    struct convert<cbag::Shape> {
        static Node encode(const cbag::Shape &rhs);

        static bool decode(const Node &node, cbag::Shape &rhs);
    };

    template<>
    struct convert<boost::variant<cbag::Rect, cbag::SchPinObject>> {
        static Node encode(const boost::variant<cbag::Rect, cbag::SchPinObject> &rhs);

        static bool decode(const Node &node, boost::variant<cbag::Rect, cbag::SchPinObject> &rhs);
    };
}


#endif //CBAG_DATABASE_YAML_FIGURES_H
