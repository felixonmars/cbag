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

#include <g3log/g3log.hpp>

#include <cbag/spirit/parsers.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/name_unit.h>

#include <cbagoa/convert.h>


namespace bsp = cbag::spirit;
namespace bsa = cbag::spirit::ast;

namespace cbagoa {

    // string parsing methinds

    bsa::name parse_name(const oa::oaString &source) {
        return cbag::parse<bsa::name, bsp::parser::name_type>(source, source.getLength(),
                                                              bsp::name());
    }

    bsa::name_unit parse_name_unit(const oa::oaString &source) {
        return cbag::parse<bsa::name_unit,
                bsp::parser::name_unit_type>(source, source.getLength(), bsp::name_unit());
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
                return {std::move(key), std::string(tmp_str)};
            }
            case oa::oacIntPropType : {
                return {std::move(key),
                        static_cast<oa::oaIntProp *>(prop_ptr)->getValue()}; // NOLINT
            }
            case oa::oacDoublePropType : {
                return {std::move(key),
                        static_cast<oa::oaDoubleProp *>(prop_ptr)->getValue()}; // NOLINT
            }
            case oa::oacFloatPropType : {
                return {std::move(key),
                        static_cast<oa::oaFloatProp *>(prop_ptr)->getValue()}; // NOLINT
            }
            case oa::oacTimePropType : {
                return {std::move(key),
                        cbag::Time(static_cast<oa::oaTimeProp *>(prop_ptr)->getValue())}; // NOLINT
            }
            case oa::oacAppPropType : {
                oa::oaByteArray data;
                static_cast<oa::oaAppProp *>(prop_ptr)->getValue(data); // NOLINT
                return {std::move(key),
                        cbag::Binary(data.getElements(), data.getNumElements())};
            }
            default : {
                throw std::invalid_argument(
                        fmt::format("Unsupported OA property {} with type: {}, see developer.",
                                    key, prop_ptr->getType().getName()));
            }
        }
    }

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

    cbag::Shape read_shape(oa::oaShape *p, const oa::oaNameSpace &ns) {
        // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
        // use NOLINT to suppress IDE warnings
        switch (p->getType()) {
            case oa::oacRectType :
                return read_rect(static_cast<oa::oaRect *>(p));  //NOLINT
            case oa::oacPolygonType :
                return read_poly(static_cast<oa::oaPolygon *>(p));  //NOLINT
            case oa::oacArcType :
                return read_arc(static_cast<oa::oaArc *>(p));  //NOLINT
            case oa::oacDonutType :
                return read_donut(static_cast<oa::oaDonut *>(p));  //NOLINT
            case oa::oacEllipseType :
                return read_ellipse(static_cast<oa::oaEllipse *>(p));  //NOLINT
            case oa::oacLineType :
                return read_line(static_cast<oa::oaLine *>(p));  //NOLINT
            case oa::oacPathType :
                return read_path(static_cast<oa::oaPath *>(p));  //NOLINT
            case oa::oacTextType :
                return read_text(static_cast<oa::oaText *>(p));  //NOLINT
            case oa::oacEvalTextType :
                return read_eval_text(static_cast<oa::oaEvalText *>(p));  //NOLINT
            case oa::oacAttrDisplayType : {
                oa::oaAttrDisplay *disp = static_cast<oa::oaAttrDisplay *>(p);
                oa::oaString text;
                disp->getText(ns, text);
                LOG(INFO) << "AttrDisplay text: " << text;
                LOG(INFO) << "AttrDisplay object type: " << disp->getObject()->getType().getName();
                return cbag::Rect();
            }
            default : {
                throw std::invalid_argument(
                        fmt::format("Unsupported OA shape type: {}, see developer.",
                                    p->getType().getName()));
            }
        }
    }

    // Read method for references

    cbag::Instance read_instance(oa::oaInst *p, const oa::oaNameSpace &ns) {
        // read cellview name
        oa::oaString inst_lib_oa, inst_cell_oa, inst_view_oa;
        p->getLibName(ns, inst_lib_oa);
        p->getCellName(ns, inst_cell_oa);
        p->getViewName(ns, inst_view_oa);

        // read transform
        cbag::Transform xform;
        p->getTransform(xform);

        // create instance object
        cbag::Instance inst(std::string(inst_lib_oa), std::string(inst_cell_oa),
                            std::string(inst_view_oa), xform);

        // read instance parameters
        if (p->hasProp()) {
            oa::oaIter<oa::oaProp> prop_iter(p->getProps());
            oa::oaProp *prop_ptr;
            while ((prop_ptr = prop_iter.getNext()) != nullptr) {
                inst.params.insert(read_prop(prop_ptr));
            }
        }

        // read instance connections
        uint32_t inst_size = p->getNumBits();
        oa::oaIter<oa::oaInstTerm> iterm_iter(p->getInstTerms(oacInstTermIterAll));
        oa::oaInstTerm *iterm_ptr;
        oa::oaString term_name_oa, net_name_oa;
        while ((iterm_ptr = iterm_iter.getNext()) != nullptr) {
            // get terminal and net names
            iterm_ptr->getTerm()->getName(ns, term_name_oa);
            iterm_ptr->getNet()->getName(ns, net_name_oa);
            bsa::name term_name = parse_name(term_name_oa);
            bsa::name net_name = parse_name(net_name_oa);

            // populate connection map
            auto tname_iter = term_name.begin();
            auto tname_end = term_name.end();
            auto nname_iter = net_name.begin();
            auto nname_end = net_name.end();
            if (inst_size == 1) {
                // handle case where we have a scalar instance
                for (; tname_iter != tname_end; ++tname_iter, ++nname_iter) {
                    if (nname_iter == nname_end) {
                        throw std::invalid_argument(
                                fmt::format("Instance terminal {} net {} length mismatch.",
                                            term_name_oa, net_name_oa));
                    }
                    bsa::name_bit term_name_bit = *tname_iter;

                    auto conn_ret = inst.connections.emplace(*tname_iter, inst_size);
                    if (conn_ret.second) {
                        (conn_ret.first->second)[0] = *nname_iter;
                    } else {
                        throw std::invalid_argument(fmt::format("Instance has duplicate pin {}",
                                                                term_name_bit.to_string()));
                    }
                }
                if (nname_iter != nname_end) {
                    throw std::invalid_argument(
                            fmt::format("Instance terminal {} net {} length mismatch.",
                                        term_name_oa, net_name_oa));
                }
            } else {
                // handle case where we have a vector instance
                std::vector<std::map<bsa::name_bit,
                        std::vector<bsa::name_bit> >::iterator> ptr_list;
                for (; tname_iter != tname_end; ++tname_iter, ++nname_iter) {
                    if (nname_iter == nname_end) {
                        throw std::invalid_argument(
                                fmt::format("Instance terminal {} net {} length mismatch.",
                                            term_name_oa, net_name_oa));
                    }
                    bsa::name_bit term_name_bit = *tname_iter;

                    auto conn_ret = inst.connections.emplace(*tname_iter, inst_size);
                    if (conn_ret.second) {
                        (conn_ret.first->second)[0] = *nname_iter;
                        ptr_list.push_back(conn_ret.first);
                    } else {
                        throw std::invalid_argument(fmt::format("Instance has duplicate pin {}",
                                                                term_name_bit.to_string()));
                    }
                }
                for (uint32_t idx = 1; idx < inst_size; ++idx) {
                    for (auto ptr : ptr_list) {
                        if (nname_iter == nname_end) {
                            throw std::invalid_argument(
                                    fmt::format("Instance {} terminal {} net {} length mismatch.",
                                                term_name_oa, net_name_oa));
                        }
                        (ptr->second)[idx] = *nname_iter;
                        ++nname_iter;
                    }
                }
            }
        }

        return inst;
    }

    std::pair<bsa::name_unit, cbag::Instance> read_instance_pair(oa::oaInst *p,
                                                                 const oa::oaNameSpace &ns) {
        oa::oaString inst_name_oa;
        p->getName(ns, inst_name_oa);
        return {parse_name_unit(inst_name_oa), read_instance(p, ns)};
    }

    // Read method for pin figures

    cbag::PinFigure read_pin_figure(oa::oaPinFig *p, const oa::oaNameSpace &ns) {
        if (p->isInst()) {
            cbag::PinFigure ans(read_instance(static_cast<oa::oaInst *>(p), ns));  // NOLINT
            return ans;
        } else if (p->getType() == oa::oacRectType) {
            cbag::PinFigure ans(read_rect(static_cast<oa::oaRect *>(p)));  // NOLINT
            return ans;
        } else {
            throw std::invalid_argument(
                    fmt::format("Unsupported OA pin figure type: {}, see developer.",
                                p->getType().getName()));
        }
    }

    // Read method for terminals

    std::pair<bsa::name, cbag::PinFigure> read_terminal_single(oa::oaTerm *term,
                                                               const oa::oaNameSpace &ns) {
        // parse terminal name
        oa::oaString term_name_oa;
        term->getName(ns, term_name_oa);

        // get pin
        oa::oaIter<oa::oaPin> pin_iter(term->getPins());
        oa::oaPin *pin_ptr = pin_iter.getNext();
        if (pin_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Terminal {} has no pins.", term_name_oa));
        }
        if (pin_iter.getNext() != nullptr) {
            throw std::invalid_argument(fmt::format("Terminal {} has more than one pin.",
                                                    term_name_oa));
        }

        // get pin figure
        oa::oaIter<oa::oaPinFig> fig_iter(pin_ptr->getFigs());
        oa::oaPinFig *fig_ptr = fig_iter.getNext();
        if (fig_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Terminal {} has no figures.", term_name_oa));
        }
        if (fig_iter.getNext() != nullptr) {
            throw std::invalid_argument(fmt::format("Terminal {} has more than one figures.",
                                                    term_name_oa));
        }

        std::pair<bsa::name, cbag::PinFigure> ans(parse_name(term_name_oa),
                                                  read_pin_figure(fig_ptr, ns));

        return ans;
    };

    // Read method for schematic/symbol cell view

    cbag::SchCellView read_sch_cell_view(oa::oaDesign *p, const oa::oaNameSpace &ns) {
        LOG(INFO) << "Reading schematic/symbol cellview";
        oa::oaBlock *block = p->getTopBlock();
        cbag::SchCellView ans;

        // read terminals
        LOG(INFO) << "Reading terminals";
        oa::oaIter<oa::oaTerm> term_iter(block->getTerms());
        oa::oaTerm *term_ptr;
        oa::oaString term_name;
        while ((term_ptr = term_iter.getNext()) != nullptr) {
            term_ptr->getName(ns, term_name);
            switch (term_ptr->getTermType()) {
                case oa::oacInputTermType :
                    ans.in_terms.insert(read_terminal_single(term_ptr, ns));
                    break;
                case oa::oacOutputTermType :
                    ans.out_terms.insert(read_terminal_single(term_ptr, ns));
                    break;
                case oa::oacInputOutputTermType :
                    ans.io_terms.insert(read_terminal_single(term_ptr, ns));
                    break;
                default:
                    throw std::invalid_argument(fmt::format("Terminal {} has invalid type: {}",
                                                            term_name,
                                                            term_ptr->getTermType().getName()));
            }
        }

        // read shapes
        LOG(INFO) << "Reading shapes";
        oa::oaIter<oa::oaShape> shape_iter(block->getShapes());
        oa::oaShape *shape_ptr;
        while ((shape_ptr = shape_iter.getNext()) != nullptr) {
            // skip shapes associated with pins.  We got those already.
            if (!shape_ptr->hasPin()) {
                ans.shapes.push_back(read_shape(shape_ptr, ns));
            }
        }

        // read instances
        LOG(INFO) << "Reading instances";
        oa::oaIter<oa::oaInst> inst_iter(block->getInsts());
        oa::oaInst *inst_ptr;
        while ((inst_ptr = inst_iter.getNext()) != nullptr) {
            // skip instances associated with pins.  We got those already.
            if (!inst_ptr->hasPin()) {
                ans.instances.insert(read_instance_pair(inst_ptr, ns));
            }
        }

        // read properties
        LOG(INFO) << "Reading properties";
        oa::oaIter<oa::oaProp> prop_iter(p->getProps());
        oa::oaProp *prop_ptr;
        while ((prop_ptr = prop_iter.getNext()) != nullptr) {
            ans.params.insert(read_prop(prop_ptr));
        }

        LOG(INFO) << "Finish reading schematic/symbol cellview";
        return ans;
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
}
