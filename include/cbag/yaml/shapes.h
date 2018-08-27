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

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::rectangle, layer, purpose, net, bbox)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::polygon, layer, purpose, net, points)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::arc, layer, purpose, net, ang_start, ang_stop, bbox)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::donut, layer, purpose, net, center, radius, hole_radius)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::ellipse, layer, purpose, net, bbox)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::line, layer, purpose, net, points)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::path, layer, purpose, net, width, points, style, begin_ext,
                          end_ext)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::text_t, layer, purpose, net, origin, alignment, orient, font,
                          height, overbar, visible, drafting, text)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::eval_text, layer, purpose, net, origin, alignment, orient,
                          font, height, overbar, visible, drafting, evaluator)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::term_attr, layer, purpose, net, origin, alignment, orient,
                          font, height, overbar, visible, drafting, attr_type, format)

namespace YAML {
template <> struct convert<cbag::sch::shape_t> {
    static Node encode(const cbag::sch::shape_t &rhs);

    static bool decode(const Node &node, cbag::sch::shape_t &rhs);
};
} // namespace YAML

#endif // CBAG_YAML_SHAPES_H
