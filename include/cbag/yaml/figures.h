/** \file yaml_figures.h
 *  \brief This file declares YAML serialization methods for figures.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_FIGURES_H
#define CBAG_YAML_FIGURES_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <yaml-cpp/yaml.h>

#include <cbag/schematic/figures.h>
#include <cbag/yaml/fusion.h>
#include <cbag/yaml/shapes.h>

BOOST_FUSION_ADAPT_STRUCT(cbag::SchInstance, lib_name, cell_name, view_name, xform, bbox,
                          connections, params)

BOOST_FUSION_ADAPT_STRUCT(cbag::SchPinObject, inst, attr)

BOOST_FUSION_ADAPT_STRUCT(cbag::PinFigure, obj, sig_type)

namespace YAML {
template <> struct convert<std::variant<cbag::Rect, cbag::SchPinObject>> {
    static Node encode(const std::variant<cbag::Rect, cbag::SchPinObject> &rhs);

    static bool decode(const Node &node, std::variant<cbag::Rect, cbag::SchPinObject> &rhs);
};
} // namespace YAML

#endif // CBAG_YAML_FIGURES_H
