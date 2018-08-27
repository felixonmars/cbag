/** \file yaml_figures.h
 *  \brief This file declares YAML serialization methods for shapes.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_SHAPES_H
#define CBAG_YAML_SHAPES_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <yaml-cpp/yaml.h>

#include <cbag/schematic/shapes.h>
#include <cbag/yaml/fusion.h>
#include <cbag/yaml/primitives.h>

BOOST_FUSION_ADAPT_STRUCT(cbag::Rect, layer, purpose, net, bbox)

BOOST_FUSION_ADAPT_STRUCT(cbag::Poly, layer, purpose, net, points)

BOOST_FUSION_ADAPT_STRUCT(cbag::Arc, layer, purpose, net, ang_start, ang_stop, bbox)

BOOST_FUSION_ADAPT_STRUCT(cbag::Donut, layer, purpose, net, center, radius, hole_radius)

BOOST_FUSION_ADAPT_STRUCT(cbag::Ellipse, layer, purpose, net, bbox)

BOOST_FUSION_ADAPT_STRUCT(cbag::Line, layer, purpose, net, points)

BOOST_FUSION_ADAPT_STRUCT(cbag::Path, layer, purpose, net, width, points, style, begin_ext, end_ext)

BOOST_FUSION_ADAPT_STRUCT(cbag::Text, layer, purpose, net, origin, alignment, orient, font, height,
                          overbar, visible, drafting, text)

BOOST_FUSION_ADAPT_STRUCT(cbag::EvalText, layer, purpose, net, origin, alignment, orient, font,
                          height, overbar, visible, drafting, evaluator)

BOOST_FUSION_ADAPT_STRUCT(cbag::TermAttr, layer, purpose, net, origin, alignment, orient, font,
                          height, overbar, visible, drafting, attr_type, format)

namespace YAML {
template <> struct convert<cbag::Shape> {
    static Node encode(const cbag::Shape &rhs);

    static bool decode(const Node &node, cbag::Shape &rhs);
};
} // namespace YAML

#endif // CBAG_YAML_SHAPES_H
