/** \file shapes_adapted.h
 *  \brief This file adapts shapes into random access data structures.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_DATABASE_SHAPES_ADAPTED_H
#define CBAG_DATABASE_SHAPES_ADAPTED_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <cbag/database/shapes.h>


BOOST_FUSION_ADAPT_STRUCT(cbag::Rect,
                          layer, purpose, net, bbox
)

BOOST_FUSION_ADAPT_STRUCT(cbag::Poly,
                          layer, purpose, net, points
)

BOOST_FUSION_ADAPT_STRUCT(cbag::Arc,
                          layer, purpose, net, ang_start, ang_stop, bbox
)

BOOST_FUSION_ADAPT_STRUCT(cbag::Donut,
                          layer, purpose, net, center, radius, hole_radius
)

BOOST_FUSION_ADAPT_STRUCT(cbag::Ellipse,
                          layer, purpose, net, bbox
)

BOOST_FUSION_ADAPT_STRUCT(cbag::Line,
                          layer, purpose, net, points
)

BOOST_FUSION_ADAPT_STRUCT(cbag::Path,
                          layer, purpose, net, width, points, style, begin_ext, end_ext
)

BOOST_FUSION_ADAPT_STRUCT(cbag::Text,
                          layer, purpose, net, origin, alignment, orient, font, height,
                          overbar, visible, drafting, text
)

BOOST_FUSION_ADAPT_STRUCT(cbag::EvalText,
                          layer, purpose, net, origin, alignment, orient, font, height,
                          overbar, visible, drafting, evaluator
)

BOOST_FUSION_ADAPT_STRUCT(cbag::TermAttr,
                          layer, purpose, net, origin, alignment, orient, font, height,
                          overbar, visible, drafting, attr_type, format
)

#endif //CBAG_DATABASE_SHAPES_ADAPTED_H
