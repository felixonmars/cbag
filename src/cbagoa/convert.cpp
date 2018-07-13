/** \file convert.cpp
 *  \brief This file implements conversion methods between OpenAccess and internal data structure.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#include <utility>

#include <fmt/format.h>
// include fmt/ostream.h to support formatting oaStrings, which defines operator <<
#include <fmt/ostream.h>

#include <cbagoa/convert.h>


namespace cbagoa {

    // Read methods for shapes

    cbag::Rect read_rect(oa::oaRect *p) {
        cbag::Rect ans(p->getLayerNum(), p->getPurposeNum());
        p->getBBox(ans.bbox);
        return ans;
    }

    cbag::Poly read_poly(oa::oaPolygon *p) {
        cbag::Poly ans(p->getLayerNum(), p->getPurposeNum(), p->getNumPoints());
        p->getPoints(ans.points);
        return ans;
    }

    cbag::Arc read_arc(oa::oaArc *p) {
        cbag::Arc ans(p->getLayerNum(), p->getPurposeNum(), p->getStartAngle(), p->getStopAngle());
        p->getEllipseBBox(ans.bbox);
        return ans;
    }

    cbag::Donut read_donut(oa::oaDonut *p) {
        cbag::Donut ans(p->getLayerNum(), p->getPurposeNum(), p->getRadius(), p->getHoleRadius());
        p->getCenter(ans.center);
        return ans;
    }

    cbag::Ellipse read_ellipse(oa::oaEllipse *p) {
        cbag::Ellipse ans(p->getLayerNum(), p->getPurposeNum());
        p->getBBox(ans.bbox);
        return ans;
    }

    cbag::Line read_line(oa::oaLine *p) {
        cbag::Line ans(p->getLayerNum(), p->getPurposeNum(), p->getNumPoints());
        p->getPoints(ans.points);
        return ans;
    }

    cbag::Path read_path(oa::oaPath *p) {
        cbag::Path ans(p->getLayerNum(), p->getPurposeNum(), p->getWidth(), p->getNumPoints(),
                       p->getStyle(), p->getBeginExt(), p->getEndExt());
        p->getPoints(ans.points);
        return ans;
    }

    cbag::Text read_text(oa::oaText *p) {
        oa::oaString text;
        p->getText(text);
        bool overbar = (p->hasOverbar() != 0);
        bool visible = (p->isVisible() != 0);
        bool drafting = (p->isDrafting() != 0);
        cbag::Text ans(p->getLayerNum(), p->getPurposeNum(), std::string(text), p->getAlignment(),
                       p->getOrient(), p->getFont(), p->getHeight(), overbar, visible, drafting);
        p->getOrigin(ans.origin);
        return ans;
    }

    cbag::EvalText read_eval_text(oa::oaEvalText *p) {
        oa::oaString text, eval;
        p->getText(text);
        p->getEvaluatorName(eval);
        bool overbar = (p->hasOverbar() != 0);
        bool visible = (p->isVisible() != 0);
        bool drafting = (p->isDrafting() != 0);
        cbag::EvalText ans(p->getLayerNum(), p->getPurposeNum(), std::string(text),
                           p->getAlignment(), p->getOrient(), p->getFont(), p->getHeight(),
                           overbar, visible, drafting, std::string(eval));
        p->getOrigin(ans.origin);
        return ans;
    }

    cbag::Shape read_shape(oa::oaShape *shape_ptr) {
        // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
        // use NOLINT to suppress IDE warnings
        switch (shape_ptr->getType()) {
            case oa::oacRectType :
                return {read_rect(static_cast<oa::oaRect *>(shape_ptr))};  //NOLINT
            case oa::oacPolygonType :
                return {read_poly(static_cast<oa::oaPolygon *>(shape_ptr))};  //NOLINT
            case oa::oacArcType :
                return {read_arc(static_cast<oa::oaArc *>(shape_ptr))};  //NOLINT
            case oa::oacDonutType :
                return {read_donut(static_cast<oa::oaDonut *>(shape_ptr))};  //NOLINT
            case oa::oacEllipseType :
                return {read_ellipse(static_cast<oa::oaEllipse *>(shape_ptr))};  //NOLINT
            case oa::oacLineType :
                return {read_line(static_cast<oa::oaLine *>(shape_ptr))};  //NOLINT
            case oa::oacPathType :
                return {read_path(static_cast<oa::oaPath *>(shape_ptr))};  //NOLINT
            case oa::oacTextType :
                return {read_text(static_cast<oa::oaText *>(shape_ptr))};  //NOLINT
            case oa::oacEvalTextType :
                return {read_eval_text(static_cast<oa::oaEvalText *>(shape_ptr))};  //NOLINT
            default :
                throw std::invalid_argument(
                        fmt::format("Unsupported OA shape type: {}, see developer.",
                                    shape_ptr->getType().getName()));
        }
    }

    // Write methods for shapes

    oa::oaRect *write_rect(oa::oaBlock *block, const cbag::Rect &v) {
        return oa::oaRect::create(block, v.layer, v.purpose, v.bbox);
    }

    oa::oaPolygon *write_poly(oa::oaBlock *block, const cbag::Poly &v) {
        return oa::oaPolygon::create(block, v.layer, v.purpose, v.points);
    }

    oa::oaArc *write_arc(oa::oaBlock *block, const cbag::Arc &v) {
        return oa::oaArc::create(block, v.layer, v.purpose, v.bbox, v.ang_start, v.ang_stop);
    }

    oa::oaDonut *write_donut(oa::oaBlock *block, const cbag::Donut &v) {
        return oa::oaDonut::create(block, v.layer, v.purpose, v.center, v.radius, v.hole_radius);
    }

    oa::oaEllipse *write_ellipse(oa::oaBlock *block, const cbag::Ellipse &v) {
        return oa::oaEllipse::create(block, v.layer, v.purpose, v.bbox);
    }

    oa::oaLine *write_line(oa::oaBlock *block, const cbag::Line &v) {
        return oa::oaLine::create(block, v.layer, v.purpose, v.points);
    }

    oa::oaPath *write_path(oa::oaBlock *block, const cbag::Path &v) {
        return oa::oaPath::create(block, v.layer, v.purpose, v.width, v.points, v.style,
                                  v.begin_ext, v.end_ext);
    }

    oa::oaText *write_text(oa::oaBlock *block, const cbag::Text &v) {
        return oa::oaText::create(block, v.layer, v.purpose, oa::oaString(v.text.c_str()),
                                  v.origin, v.alignment, v.orient, v.font, v.height, v.overbar,
                                  v.visible, v.drafting);
    }

    oa::oaEvalText *write_eval_text(oa::oaBlock *block, const cbag::EvalText &v) {
        return oa::oaEvalText::create(block, v.layer, v.purpose, oa::oaString(v.text.c_str()),
                                      v.origin, v.alignment, v.orient, v.font, v.height,
                                      oa::oaString(v.evaluator.c_str()), v.overbar,
                                      v.visible, v.drafting);
    }

    // Read method for properties

    std::pair<std::string, cbag::value_t> read_prop(oa::oaProp *prop_ptr) {
        oa::oaString tmp_str;
        prop_ptr->getName(tmp_str);
        std::string key(tmp_str);
        // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
        // use NOLINT to suppress IDE warnings
        switch (prop_ptr->getType()) {
            case oa::oacStringPropType : {
                prop_ptr->getValue(tmp_str);
                return {key, cbag::value_t(std::string(tmp_str))};
            }
            case oa::oacIntPropType : {
                return {key, cbag::value_t(
                        static_cast<oa::oaIntProp *>(prop_ptr)->getValue())}; // NOLINT
            }
            case oa::oacDoublePropType : {
                return {key, cbag::value_t(
                        static_cast<oa::oaDoubleProp *>(prop_ptr)->getValue())}; // NOLINT
            }
            case oa::oacFloatPropType : {
                return {key, cbag::value_t(
                        static_cast<oa::oaFloatProp *>(prop_ptr)->getValue())}; // NOLINT
            }
            case oa::oacTimePropType : {
                return {key, cbag::value_t(cbag::Time(
                        static_cast<oa::oaTimeProp *>(prop_ptr)->getValue()))}; // NOLINT
            }
            case oa::oacAppPropType : {
                oa::oaByteArray data;
                static_cast<oa::oaAppProp *>(prop_ptr)->getValue(data); // NOLINT
                return {key, cbag::value_t(cbag::Binary(data.getElements(),
                                                        data.getNumElements()))};
            }
            default : {
                throw std::invalid_argument(
                        fmt::format("Unsupported OA property {} with type: {}, see developer.",
                                    key, prop_ptr->getType().getName()));
            }
        }

    }
}
