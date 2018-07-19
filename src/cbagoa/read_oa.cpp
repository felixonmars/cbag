/** \file read_oa.cpp
 *  \brief This file converts OpenAccess objects to CBAG data structure.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#include <utility>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <easylogging++.h>

#include <cbag/spirit/parsers.h>
#include <cbag/spirit/name.h>
#include <cbagoa/read_oa.h>


namespace bsp = cbag::spirit;
namespace bsa = cbag::spirit::ast;

namespace cbagoa {

    // Read method for properties

    std::pair<std::string, cbag::value_t> OAReader::read_prop(oa::oaProp *p) {
        oa::oaString tmp_str;
        p->getName(tmp_str);
        std::string key(tmp_str);
        // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
        // use NOLINT to suppress IDE warnings
        switch (p->getType()) {
            case oa::oacStringPropType : {
                p->getValue(tmp_str);
                return {std::move(key), std::string(tmp_str)};
            }
            case oa::oacIntPropType : {
                return {std::move(key),
                        static_cast<oa::oaIntProp *>(p)->getValue()}; // NOLINT
            }
            case oa::oacDoublePropType : {
                return {std::move(key),
                        static_cast<oa::oaDoubleProp *>(p)->getValue()}; // NOLINT
            }
            case oa::oacTimePropType : {
                return {std::move(key),
                        cbag::Time(static_cast<oa::oaTimeProp *>(p)->getValue())}; // NOLINT
            }
            case oa::oacAppPropType : {
                oa::oaByteArray data;
                oa::oaString app_str;
                oa::oaAppProp *app_ptr = static_cast<oa::oaAppProp *>(p); // NOLINT
                app_ptr->getValue(data); // NOLINT
                app_ptr->getAppType(app_str);
                return {std::move(key),
                        cbag::Binary(app_str, data.getElements(), data.getNumElements())};
            }
            case oa::oacBooleanPropType : {
                return {std::move(key), static_cast<bool>(static_cast<oa::oaBooleanProp *>(p)->getValue())};
            }
            default : {
                throw std::invalid_argument(
                        fmt::format("Unsupported OA property {} with type: {}, see developer.",
                                    key, p->getType().getName()));
            }
        }
    }

    std::pair<std::string, cbag::value_t>
    OAReader::read_app_def(oa::oaDesign *dsn, oa::oaAppDef *p) {
        oa::oaString tmp_str;
        p->getName(tmp_str);
        std::string key(tmp_str);
        // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
        // use NOLINT to suppress IDE warnings
        switch (p->getType()) {
            case oa::oacIntAppDefType : {
                return {std::move(key),
                        (static_cast<oa::oaIntAppDef<oa::oaDesign> *>(p))->get(dsn)}; // NOLINT
            }
            case oa::oacStringAppDefType : {
                (static_cast<oa::oaStringAppDef<oa::oaDesign> *>(p))->get(dsn, tmp_str); // NOLINT
                return {std::move(key), std::string(tmp_str)};
            }
            default : {
                throw std::invalid_argument(
                        fmt::format("Unsupported OA AppDef {} with type: {}, see developer.",
                                    key, p->getType().getName()));
            }
        }
    }

    // Read methods for shapes

    cbag::Rect OAReader::read_rect(oa::oaRect *p, std::string &&net) {
        cbag::Rect ans(p->getLayerNum(), p->getPurposeNum(), net);
        p->getBBox(ans.bbox);
        return ans;
    }

    cbag::Poly OAReader::read_poly(oa::oaPolygon *p, std::string &&net) {
        cbag::Poly ans(p->getLayerNum(), p->getPurposeNum(), net, p->getNumPoints());
        p->getPoints(ans.points);
        return ans;
    }

    cbag::Arc OAReader::read_arc(oa::oaArc *p, std::string &&net) {
        cbag::Arc ans(p->getLayerNum(), p->getPurposeNum(), net, p->getStartAngle(),
                      p->getStopAngle());
        p->getEllipseBBox(ans.bbox);
        return ans;
    }

    cbag::Donut OAReader::read_donut(oa::oaDonut *p, std::string &&net) {
        cbag::Donut ans(p->getLayerNum(), p->getPurposeNum(), net, p->getRadius(),
                        p->getHoleRadius());
        p->getCenter(ans.center);
        return ans;
    }

    cbag::Ellipse OAReader::read_ellipse(oa::oaEllipse *p, std::string &&net) {
        cbag::Ellipse ans(p->getLayerNum(), p->getPurposeNum(), net);
        p->getBBox(ans.bbox);
        return ans;
    }

    cbag::Line OAReader::read_line(oa::oaLine *p, std::string &&net) {
        cbag::Line ans(p->getLayerNum(), p->getPurposeNum(), net, p->getNumPoints());
        p->getPoints(ans.points);
        return ans;
    }

    cbag::Path OAReader::read_path(oa::oaPath *p, std::string &&net) {
        cbag::Path ans(p->getLayerNum(), p->getPurposeNum(), net, p->getWidth(), p->getNumPoints(),
                       p->getStyle(), p->getBeginExt(), p->getEndExt());
        p->getPoints(ans.points);
        return ans;
    }

    cbag::Text OAReader::read_text(oa::oaText *p, std::string &&net) {
        oa::oaString text;
        p->getText(text);
        bool overbar = (p->hasOverbar() != 0);
        bool visible = (p->isVisible() != 0);
        bool drafting = (p->isDrafting() != 0);
        cbag::Text ans(p->getLayerNum(), p->getPurposeNum(), net, std::string(text),
                       p->getAlignment(), p->getOrient(), p->getFont(), p->getHeight(), overbar,
                       visible, drafting);
        p->getOrigin(ans.origin);
        return ans;
    }

    cbag::EvalText OAReader::read_eval_text(oa::oaEvalText *p, std::string &&net) {
        oa::oaString text, eval;
        p->getText(text);
        p->getEvaluatorName(eval);
        bool overbar = (p->hasOverbar() != 0);
        bool visible = (p->isVisible() != 0);
        bool drafting = (p->isDrafting() != 0);
        cbag::EvalText ans(p->getLayerNum(), p->getPurposeNum(), net, std::string(text),
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
        std::string net;
        if (p->hasNet()) {
            oa::oaString net_name;
            p->getNet()->getName(ns, net_name);
            net = std::string(net_name);
        }

        // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
        // use NOLINT to suppress IDE warnings
        switch (p->getType()) {
            case oa::oacRectType :
                return read_rect(static_cast<oa::oaRect *>(p), std::move(net));  //NOLINT
            case oa::oacPolygonType :
                return read_poly(static_cast<oa::oaPolygon *>(p), std::move(net));  //NOLINT
            case oa::oacArcType :
                return read_arc(static_cast<oa::oaArc *>(p), std::move(net));  //NOLINT
            case oa::oacDonutType :
                return read_donut(static_cast<oa::oaDonut *>(p), std::move(net));  //NOLINT
            case oa::oacEllipseType :
                return read_ellipse(static_cast<oa::oaEllipse *>(p), std::move(net));  //NOLINT
            case oa::oacLineType :
                return read_line(static_cast<oa::oaLine *>(p), std::move(net));  //NOLINT
            case oa::oacPathType :
                return read_path(static_cast<oa::oaPath *>(p), std::move(net));  //NOLINT
            case oa::oacTextType :
                return read_text(static_cast<oa::oaText *>(p), std::move(net));  //NOLINT
            case oa::oacEvalTextType :
                return read_eval_text(static_cast<oa::oaEvalText *>(p), std::move(net));  //NOLINT
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
        LOG(INFO) << "Reading connections";
        oa::oaIter<oa::oaInstTerm> iterm_iter(p->getInstTerms(oacInstTermIterAll));
        oa::oaInstTerm *iterm_ptr;
        oa::oaString term_name_oa, net_name_oa;
        while ((iterm_ptr = iterm_iter.getNext()) != nullptr) {
            // get terminal and net names
            iterm_ptr->getTermName(ns, term_name_oa);
            iterm_ptr->getNet()->getName(ns, net_name_oa);
            LOG(INFO) << "Terminal " << term_name_oa << " connected to net " << net_name_oa;
            inst.connections.emplace(std::string(term_name_oa), std::string(net_name_oa));
        }

        return inst;
    }

    std::pair<std::string, cbag::Instance> OAReader::read_instance_pair(oa::oaInst *p) {
        oa::oaString inst_name_oa;
        p->getName(ns, inst_name_oa);
        LOG(INFO) << "Reading instance " << inst_name_oa;
        return {std::string(inst_name_oa), read_instance(p)};
    }

    // Read method for pin figures

    cbag::PinFigure OAReader::read_pin_figure(oa::oaTerm *t, oa::oaPinFig *p) {
        cbag::SigType sig = t->getNet()->getSigType();
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
            std::string net;
            if (disp_ptr->hasNet()) {
                oa::oaString net_name;
                disp_ptr->getNet()->getName(ns, net_name);
                net = std::string(net_name);
            }
            cbag::TermAttr attr(
                    oa::oaTermAttrType(disp_ptr->getAttribute().getRawValue()).getValue(),
                    disp_ptr->getLayerNum(), disp_ptr->getPurposeNum(), net,
                    disp_ptr->getAlignment(), disp_ptr->getOrient(),
                    disp_ptr->getFont(), disp_ptr->getHeight(), disp_ptr->getFormat(),
                    overbar, visible, drafting);
            disp_ptr->getOrigin(attr.origin);

            return {cbag::SchPinObject(std::move(inst), std::move(attr)), sig};
        } else if (p->getType() == oa::oacRectType) {
            oa::oaRect *r = static_cast<oa::oaRect *>(p); // NOLINT
            std::string net;
            if (r->hasNet()) {
                oa::oaString tmp;
                r->getNet()->getName(ns, tmp);
                net = std::string(tmp);
            }
            return {read_rect(r, std::move(net)), sig};
        } else {
            throw std::invalid_argument(
                    fmt::format("Unsupported OA pin figure type: {}, see developer.",
                                p->getType().getName()));
        }
    }

    // Read method for terminals

    std::pair<std::string, cbag::PinFigure> OAReader::read_terminal_single(oa::oaTerm *term) {
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

        return {std::string(term_name_oa), read_pin_figure(term, fig_ptr)};
    };

    // Read method for schematic/symbol cell view

    cbag::SchCellView OAReader::read_sch_cellview(oa::oaDesign *p) {
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
        LOG(INFO) << "Reading shapes";
        oa::oaIter<oa::oaShape> shape_iter(block->getShapes());
        oa::oaShape *shape_ptr;
        while ((shape_ptr = shape_iter.getNext()) != nullptr) {
            LOG(INFO) << "shape type: " << shape_ptr->getType().getName();
            // skip shapes associated with pins.  We got those already.
            if (include_shape(shape_ptr)) {
                ans.shapes.push_back(read_shape(shape_ptr));
            } else {
                LOG(INFO) << "Skipping";
            }
        }

        // read instances
        LOG(INFO) << "Reading instances";
        oa::oaIter<oa::oaInst> inst_iter(block->getInsts());
        oa::oaInst *inst_ptr;
        while ((inst_ptr = inst_iter.getNext()) != nullptr) {
            // skip instances associated with pins.  We got those already.
            if (!inst_ptr->hasPin()) {
                ans.instances.insert(read_instance_pair(inst_ptr));
            }
        }

        // read properties
        LOG(INFO) << "Reading properties";
        oa::oaIter<oa::oaProp> prop_iter(p->getProps());
        oa::oaProp *prop_ptr;
        while ((prop_ptr = prop_iter.getNext()) != nullptr) {
            ans.props.insert(read_prop(prop_ptr));
        }

        LOG(INFO) << "Reading AppDefs";
        oa::oaIter<oa::oaAppDef> appdef_iter(p->getAppDefs());
        oa::oaAppDef *appdef_ptr;
        while ((appdef_ptr = appdef_iter.getNext()) != nullptr) {
            ans.app_defs.insert(read_app_def(p, appdef_ptr));
        }

        LOG(INFO) << "Reading design groups";
        oa::oaIter<oa::oaGroup> grp_iter(p->getGroups(
                oacGroupIterBlockDomain | oacGroupIterModDomain | oacGroupIterNoDomain |
                oacGroupIterOccDomain));
        oa::oaGroup *grp_ptr;
        while ((grp_ptr = grp_iter.getNext()) != nullptr) {
            oa::oaString grp_str;
            grp_ptr->getName(grp_str);
            LOG(INFO) << "group name: " << grp_str << ", domain: "
                      << grp_ptr->getGroupDomain().getName();
            LOG(INFO) << "group has prop: " << grp_ptr->hasProp() << ", has appdef: "
                      << grp_ptr->hasAppDef();
            grp_ptr->getDef()->getName(grp_str);
            LOG(INFO) << "group def name: " << grp_str;
            oa::oaIter<oa::oaGroupMember> mem_iter(grp_ptr->getMembers());
            oa::oaGroupMember *mem_ptr;
            while ((mem_ptr = mem_iter.getNext()) != nullptr) {
                LOG(INFO) << "group object type: " << mem_ptr->getObject()->getType().getName();
            }
        }

        LOG(INFO) << "Finish reading schematic/symbol cellview";
        return ans;
    }
}
