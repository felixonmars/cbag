/** \file read_oa.cpp
 *  \brief This file converts OpenAccess objects to CBAG data structure.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#include <utility>

#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/ostream.h>

#include <cbag/spirit/parsers.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/name_unit.h>

#include <cbagoa/name.h>
#include <cbagoa/read_oa.h>


namespace bsp = cbag::spirit;
namespace bsa = cbag::spirit::ast;

namespace cbagoa {

    // string parsing methinds

    bsa::name OAReader::parse_name(const oa::oaString &source) {
        return cbag::parse<bsa::name, bsp::parser::name_type>(source, source.getLength(),
                                                              bsp::name());
    }

    bsa::name_unit OAReader::parse_name_unit(const oa::oaString &source) {
        return cbag::parse<bsa::name_unit,
                bsp::parser::name_unit_type>(source, source.getLength(), bsp::name_unit());
    }

    // Read method for properties

    std::pair<std::string, cbag::value_t> OAReader::read_prop(oa::oaProp *prop_ptr) {
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
            case oa::oacTimePropType : {
                return {std::move(key),
                        cbag::Time(static_cast<oa::oaTimeProp *>(prop_ptr)->getValue())}; // NOLINT
            }
            case oa::oacAppPropType : {
                oa::oaByteArray data;
                oa::oaString app_str;
                oa::oaAppProp *app_ptr = static_cast<oa::oaAppProp *>(prop_ptr);
                app_ptr->getValue(data); // NOLINT
                app_ptr->getAppType(app_str);
                return {std::move(key),
                        cbag::Binary(app_str, data.getElements(), data.getNumElements())};
            }
            default : {
                throw std::invalid_argument(
                        fmt::format("Unsupported OA property {} with type: {}, see developer.",
                                    key, prop_ptr->getType().getName()));
            }
        }
    }

    // Read methods for shapes

    cbag::Rect OAReader::read_rect(oa::oaRect *p) {
        cbag::Rect ans(p->getLayerNum(), p->getPurposeNum());
        p->getBBox(ans.bbox);
        return ans;
    }

    cbag::Poly OAReader::read_poly(oa::oaPolygon *p) {
        cbag::Poly ans(p->getLayerNum(), p->getPurposeNum(), p->getNumPoints());
        p->getPoints(ans.points);
        return ans;
    }

    cbag::Arc OAReader::read_arc(oa::oaArc *p) {
        cbag::Arc ans(p->getLayerNum(), p->getPurposeNum(), p->getStartAngle(), p->getStopAngle());
        p->getEllipseBBox(ans.bbox);
        return ans;
    }

    cbag::Donut OAReader::read_donut(oa::oaDonut *p) {
        cbag::Donut ans(p->getLayerNum(), p->getPurposeNum(), p->getRadius(), p->getHoleRadius());
        p->getCenter(ans.center);
        return ans;
    }

    cbag::Ellipse OAReader::read_ellipse(oa::oaEllipse *p) {
        cbag::Ellipse ans(p->getLayerNum(), p->getPurposeNum());
        p->getBBox(ans.bbox);
        return ans;
    }

    cbag::Line OAReader::read_line(oa::oaLine *p) {
        cbag::Line ans(p->getLayerNum(), p->getPurposeNum(), p->getNumPoints());
        p->getPoints(ans.points);
        return ans;
    }

    cbag::Path OAReader::read_path(oa::oaPath *p) {
        cbag::Path ans(p->getLayerNum(), p->getPurposeNum(), p->getWidth(), p->getNumPoints(),
                       p->getStyle(), p->getBeginExt(), p->getEndExt());
        p->getPoints(ans.points);
        return ans;
    }

    cbag::Text OAReader::read_text(oa::oaText *p) {
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

    cbag::EvalText OAReader::read_eval_text(oa::oaEvalText *p) {
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

    /** Returns true if the given shape should be included.
     *
     *  The rules are:
     *  1. if a shape has a pin, don't include it (we already added it to the pins).
     *  2. if a shape is an attribute display of a terminal, don't include it (we already added it).
     *  3. otherwise, include it.
     */
    bool include_shape(oa::oaShape *p) {
        if (!p->hasPin()) {
            if (p->getType() == oa::oacAttrDisplayType) {
                auto disp = static_cast<oa::oaAttrDisplay *>(p); // NOLINT
                if (disp->getObject()->isDesign()) {
                    auto obj = static_cast<oa::oaDesignObject *>(disp->getObject()); // NOLINT
                    if (obj->isBlockObject()) {
                        return !((static_cast<oa::oaBlockObject *>(obj))->isTerm()); // NOLINT
                    } else {
                        return true;
                    }
                } else {
                    return true;
                }
            } else {
                return true;
            }
        } else {
            return false;
        }
    }

    cbag::Shape OAReader::read_shape(oa::oaShape *p) {
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
            default : {
                throw std::invalid_argument(
                        fmt::format("Unsupported OA shape type: {}, see developer.",
                                    p->getType().getName()));
            }
        }
    }

    // Read method for references

    cbag::Instance OAReader::read_instance(oa::oaInst *p) {
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
                                                                to_string(term_name_bit)));
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
                                                                to_string(term_name_bit)));
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

    std::pair<bsa::name_unit, cbag::Instance> OAReader::read_instance_pair(oa::oaInst *p) {
        oa::oaString inst_name_oa;
        p->getName(ns, inst_name_oa);
        return {parse_name_unit(inst_name_oa), read_instance(p)};
    }

    // Read method for pin figures

    cbag::PinFigure OAReader::read_pin_figure(oa::oaTerm *t, oa::oaPinFig *p) {
        if (p->isInst()) {
            cbag::Instance inst = read_instance(static_cast<oa::oaInst *>(p)); // NOLINT

            oa::oaTextDisplayIter disp_iter(oa::oaTextDisplay::getTextDisplays(t));
            auto *disp_ptr = static_cast<oa::oaAttrDisplay *>(disp_iter.getNext()); // NOLINT
            if (disp_ptr == nullptr) {
                throw std::invalid_argument(fmt::format("Terminal has no attr display."));
            }
            if (disp_iter.getNext() != nullptr) {
                throw std::invalid_argument(
                        fmt::format("Terminal has more than one attr display."));
            }

            bool overbar = (disp_ptr->hasOverbar() != 0);
            bool visible = (disp_ptr->isVisible() != 0);
            bool drafting = (disp_ptr->isDrafting() != 0);
            cbag::TermAttr attr(
                    oa::oaTermAttrType(disp_ptr->getAttribute().getRawValue()).getValue(),
                    disp_ptr->getLayerNum(), disp_ptr->getPurposeNum(),
                    disp_ptr->getAlignment(), disp_ptr->getOrient(),
                    disp_ptr->getFont(), disp_ptr->getHeight(), disp_ptr->getFormat(),
                    overbar, visible, drafting);
            disp_ptr->getOrigin(attr.origin);

            return cbag::SchPinObject(std::move(inst), std::move(attr));
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

    std::pair<bsa::name, cbag::PinFigure> OAReader::read_terminal_single(oa::oaTerm *term) {
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
                                                  read_pin_figure(term, fig_ptr));

        return ans;
    };

    // Read method for schematic/symbol cell view

    cbag::SchCellView OAReader::read_sch_cellview(oa::oaDesign *p) {
        oa::oaBlock *block = p->getTopBlock();
        cbag::SchCellView ans;

        // read terminals
        logger->info("Reading terminals");
        oa::oaIter<oa::oaTerm> term_iter(block->getTerms());
        oa::oaTerm *term_ptr;
        oa::oaString term_name;
        while ((term_ptr = term_iter.getNext()) != nullptr) {
            term_ptr->getName(ns, term_name);
            switch (term_ptr->getTermType()) {
                case oa::oacInputTermType :
                    ans.in_terms.insert(read_terminal_single(term_ptr));
                    break;
                case oa::oacOutputTermType :
                    ans.out_terms.insert(read_terminal_single(term_ptr));
                    break;
                case oa::oacInputOutputTermType :
                    ans.io_terms.insert(read_terminal_single(term_ptr));
                    break;
                default:
                    throw std::invalid_argument(fmt::format("Terminal {} has invalid type: {}",
                                                            term_name,
                                                            term_ptr->getTermType().getName()));
            }
        }

        // read shapes
        logger->info("Reading shapes");
        oa::oaIter<oa::oaShape> shape_iter(block->getShapes());
        oa::oaShape *shape_ptr;
        while ((shape_ptr = shape_iter.getNext()) != nullptr) {
            // skip shapes associated with pins.  We got those already.
            if (include_shape(shape_ptr)) {
                ans.shapes.push_back(read_shape(shape_ptr));
            }
        }

        // read instances
        logger->info("Reading instances");
        oa::oaIter<oa::oaInst> inst_iter(block->getInsts());
        oa::oaInst *inst_ptr;
        while ((inst_ptr = inst_iter.getNext()) != nullptr) {
            // skip instances associated with pins.  We got those already.
            if (!inst_ptr->hasPin()) {
                ans.instances.insert(read_instance_pair(inst_ptr));
            }
        }

        // read properties
        logger->info("Reading properties");
        oa::oaIter<oa::oaProp> prop_iter(p->getProps());
        oa::oaProp *prop_ptr;
        while ((prop_ptr = prop_iter.getNext()) != nullptr) {
            ans.params.insert(read_prop(prop_ptr));
        }

        logger->info("Finish reading schematic/symbol cellview");
        return ans;
    }
}
