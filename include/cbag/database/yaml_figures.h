/** \file yaml_figures.h
 *  \brief This file declares YAML serialization methods for figures.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_DATABASE_YAML_FIGURES_H
#define CBAG_DATABASE_YAML_FIGURES_H

#include <yaml-cpp/yaml.h>

#include <cbag/database/figures_adapted.h>
#include <cbag/database/yaml_shapes.h>
#include <cbag/yaml_fusion.h>

namespace YAML {
template <> struct convert<boost::variant<cbag::Rect, cbag::SchPinObject>> {
    static Node
    encode(const boost::variant<cbag::Rect, cbag::SchPinObject> &rhs);

    static bool decode(const Node &node,
                       boost::variant<cbag::Rect, cbag::SchPinObject> &rhs);
};
} // namespace YAML

#endif // CBAG_DATABASE_YAML_FIGURES_H
