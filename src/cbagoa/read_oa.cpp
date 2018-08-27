/** \file read_oa.cpp
 *  \brief This file converts OpenAccess objects to CBAG data structure.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#include <utility>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <cbag/cbag.h>
#include <cbagoa/read_oa.h>

namespace cbagoa {

// Read method for properties

std::pair<std::string, cbag::value_t> OAReader::read_prop(oa::oaProp *p) {
    oa::oaString tmp_str;
    p->getName(tmp_str);
    std::string key(tmp_str);
    // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
    switch (p->getType()) {
    case oa::oacStringPropType: {
        p->getValue(tmp_str);
        return {std::move(key), std::string(tmp_str)};
    }
    case oa::oacIntPropType: {
        return {std::move(key), static_cast<oa::oaIntProp *>(p)->getValue()};
    }
    case oa::oacDoublePropType: {
        return {std::move(key), static_cast<oa::oaDoubleProp *>(p)->getValue()};
    }
    case oa::oacTimePropType: {
        return {std::move(key), cbag::time_struct(static_cast<oa::oaTimeProp *>(p)->getValue())};
    }
    case oa::oacAppPropType: {
        oa::oaByteArray data;
        oa::oaString app_str;
        oa::oaAppProp *app_ptr = static_cast<oa::oaAppProp *>(p);
        app_ptr->getValue(data);
        app_ptr->getAppType(app_str);
        return {std::move(key), cbag::binary_t(app_str, data.getElements(), data.getNumElements())};
    }
    case oa::oacBooleanPropType: {
        return {std::move(key), static_cast<bool>(static_cast<oa::oaBooleanProp *>(p)->getValue())};
    }
    default: {
        throw std::invalid_argument(
            fmt::format("Unsupported OA property {} with type: {}, see developer.", key,
                        (const char *)p->getType().getName()));
    }
    }
}

std::pair<std::string, cbag::value_t> OAReader::read_app_def(oa::oaDesign *dsn, oa::oaAppDef *p) {
    oa::oaString tmp_str;
    p->getName(tmp_str);
    std::string key(tmp_str);
    // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
    switch (p->getType()) {
    case oa::oacIntAppDefType: {
        return {std::move(key), (static_cast<oa::oaIntAppDef<oa::oaDesign> *>(p))->get(dsn)};
    }
    case oa::oacStringAppDefType: {
        (static_cast<oa::oaStringAppDef<oa::oaDesign> *>(p))->get(dsn, tmp_str);
        return {std::move(key), std::string(tmp_str)};
    }
    default: {
        throw std::invalid_argument(
            fmt::format("Unsupported OA AppDef {} with type: {}, see developer.", key,
                        (const char *)p->getType().getName()));
    }
    }
}

// Read methods for shapes

cbag::sch::rectangle OAReader::read_rect(oa::oaRect *p, std::string &&net) {
    cbag::sch::rectangle ans(p->getLayerNum(), p->getPurposeNum(), net);
    p->getBBox(ans.bbox);
    return ans;
}

cbag::sch::polygon OAReader::read_poly(oa::oaPolygon *p, std::string &&net) {
    cbag::sch::polygon ans(p->getLayerNum(), p->getPurposeNum(), net, p->getNumPoints());
    p->getPoints(ans.points);
    return ans;
}

cbag::sch::arc OAReader::read_arc(oa::oaArc *p, std::string &&net) {
    cbag::sch::arc ans(p->getLayerNum(), p->getPurposeNum(), net, p->getStartAngle(),
                       p->getStopAngle());
    p->getEllipseBBox(ans.bbox);
    return ans;
}

cbag::sch::donut OAReader::read_donut(oa::oaDonut *p, std::string &&net) {
    cbag::sch::donut ans(p->getLayerNum(), p->getPurposeNum(), net, p->getRadius(),
                         p->getHoleRadius());
    p->getCenter(ans.center);
    return ans;
}

cbag::sch::ellipse OAReader::read_ellipse(oa::oaEllipse *p, std::string &&net) {
    cbag::sch::ellipse ans(p->getLayerNum(), p->getPurposeNum(), net);
    p->getBBox(ans.bbox);
    return ans;
}

cbag::sch::line OAReader::read_line(oa::oaLine *p, std::string &&net) {
    cbag::sch::line ans(p->getLayerNum(), p->getPurposeNum(), net, p->getNumPoints());
    p->getPoints(ans.points);
    return ans;
}

cbag::sch::path OAReader::read_path(oa::oaPath *p, std::string &&net) {
    cbag::sch::path ans(p->getLayerNum(), p->getPurposeNum(), net, p->getWidth(), p->getNumPoints(),
                        p->getStyle(), p->getBeginExt(), p->getEndExt());
    p->getPoints(ans.points);
    return ans;
}

cbag::sch::text_t OAReader::read_text(oa::oaText *p, std::string &&net) {
    oa::oaString text;
    p->getText(text);
    bool overbar = (p->hasOverbar() != 0);
    bool visible = (p->isVisible() != 0);
    bool drafting = (p->isDrafting() != 0);
    cbag::sch::text_t ans(p->getLayerNum(), p->getPurposeNum(), net, std::string(text),
                          p->getAlignment(), p->getOrient(), p->getFont(), p->getHeight(), overbar,
                          visible, drafting);
    p->getOrigin(ans.origin);
    return ans;
}

cbag::sch::eval_text OAReader::read_eval_text(oa::oaEvalText *p, std::string &&net) {
    oa::oaString text, eval;
    p->getText(text);
    p->getEvaluatorName(eval);
    bool overbar = (p->hasOverbar() != 0);
    bool visible = (p->isVisible() != 0);
    bool drafting = (p->isDrafting() != 0);
    cbag::sch::eval_text ans(p->getLayerNum(), p->getPurposeNum(), net, std::string(text),
                             p->getAlignment(), p->getOrient(), p->getFont(), p->getHeight(),
                             overbar, visible, drafting, std::string(eval));
    p->getOrigin(ans.origin);
    return ans;
}

/** Returns true if the given shape should be included.
 *
 *  The rules are:
 *  1. if a shape has a pin, don't include it (we already added it to the pins).
 *  2. if a shape is an attribute display of a terminal, don't include it (we
 * already added it).
 *  3. otherwise, include it.
 */
bool include_shape(oa::oaShape *p) {
    if (!p->hasPin()) {
        if (p->getType() == oa::oacAttrDisplayType) {
            auto disp = static_cast<oa::oaAttrDisplay *>(p);
            if (disp->getObject()->isDesign()) {
                auto obj = static_cast<oa::oaDesignObject *>(disp->getObject());
                if (obj->isBlockObject()) {
                    return !((static_cast<oa::oaBlockObject *>(obj))->isTerm());
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

cbag::sch::shape_t OAReader::read_shape(oa::oaShape *p) {
    std::string net;
    if (p->hasNet()) {
        oa::oaString net_name;
        p->getNet()->getName(ns, net_name);
        net = std::string(net_name);
        logger->info("Shape associated with net: {}", net);
    } else {
        logger->info("Shape has no net");
    }

    // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
    switch (p->getType()) {
    case oa::oacRectType:
        return read_rect(static_cast<oa::oaRect *>(p), std::move(net));
    case oa::oacPolygonType:
        return read_poly(static_cast<oa::oaPolygon *>(p), std::move(net));
    case oa::oacArcType:
        return read_arc(static_cast<oa::oaArc *>(p), std::move(net));
    case oa::oacDonutType:
        return read_donut(static_cast<oa::oaDonut *>(p), std::move(net));
    case oa::oacEllipseType:
        return read_ellipse(static_cast<oa::oaEllipse *>(p), std::move(net));
    case oa::oacLineType:
        return read_line(static_cast<oa::oaLine *>(p), std::move(net));
    case oa::oacPathType:
        return read_path(static_cast<oa::oaPath *>(p), std::move(net));
    case oa::oacTextType:
        return read_text(static_cast<oa::oaText *>(p), std::move(net));
    case oa::oacEvalTextType:
        return read_eval_text(static_cast<oa::oaEvalText *>(p), std::move(net));
    default: {
        throw std::invalid_argument(fmt::format("Unsupported OA shape type: {}, see developer.",
                                                (const char *)p->getType().getName()));
    }
    }
}

// Read method for references

cbag::sch::instance OAReader::read_instance(oa::oaInst *p) {
    // read cellview name
    oa::oaString inst_lib_oa, inst_cell_oa, inst_view_oa;
    p->getLibName(ns, inst_lib_oa);
    p->getCellName(ns, inst_cell_oa);
    p->getViewName(ns, inst_view_oa);

    // read transform and bounding box
    oa::oaTransform xform;
    p->getTransform(xform);
    oa::oaBox bbox;
    p->getBBox(bbox);

    // create instance object
    cbag::sch::instance inst(std::string(inst_lib_oa), std::string(inst_cell_oa),
                             std::string(inst_view_oa), xform, bbox);

    // read instance parameters
    if (p->hasProp()) {
        oa::oaIter<oa::oaProp> prop_iter(p->getProps());
        oa::oaProp *prop_ptr;
        while ((prop_ptr = prop_iter.getNext()) != nullptr) {
            inst.params.insert(read_prop(prop_ptr));
        }
    }

    // read instance connections
    logger->info("Reading connections");
    oa::oaIter<oa::oaInstTerm> iterm_iter(p->getInstTerms(oacInstTermIterNotImplicit));
    oa::oaInstTerm *iterm_ptr;
    oa::oaString term_name_oa, net_name_oa;
    while ((iterm_ptr = iterm_iter.getNext()) != nullptr) {
        // get terminal and net names
        iterm_ptr->getTermName(ns, term_name_oa);
        iterm_ptr->getNet()->getName(ns, net_name_oa);
        logger->info("Terminal {} connected to net {}", (const char *)term_name_oa,
                     (const char *)net_name_oa);
        inst.connections.emplace(std::string(term_name_oa), std::string(net_name_oa));
    }

    return inst;
}

std::pair<std::string, cbag::sch::instance> OAReader::read_instance_pair(oa::oaInst *p) {
    oa::oaString inst_name_oa;
    p->getName(ns, inst_name_oa);
    logger->info("Reading instance {}", (const char *)inst_name_oa);
    return {std::string(inst_name_oa), read_instance(p)};
}

// Read method for pin figures

cbag::sch::pin_figure OAReader::read_pin_figure(oa::oaTerm *t, oa::oaPinFig *p) {
    oa::oaSigType sig = t->getNet()->getSigType();
    if (p->isInst()) {
        cbag::sch::instance inst = read_instance(static_cast<oa::oaInst *>(p));

        oa::oaTextDisplayIter disp_iter(oa::oaTextDisplay::getTextDisplays(t));
        auto *disp_ptr = static_cast<oa::oaAttrDisplay *>(disp_iter.getNext());
        if (disp_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Terminal has no attr display."));
        }
        if (disp_iter.getNext() != nullptr) {
            throw std::invalid_argument(fmt::format("Terminal has more than one attr display."));
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
        cbag::sch::term_attr attr(
            oa::oaTermAttrType(disp_ptr->getAttribute().getRawValue()).getValue(),
            disp_ptr->getLayerNum(), disp_ptr->getPurposeNum(), net, disp_ptr->getAlignment(),
            disp_ptr->getOrient(), disp_ptr->getFont(), disp_ptr->getHeight(),
            disp_ptr->getFormat(), overbar, visible, drafting);
        disp_ptr->getOrigin(attr.origin);

        return {cbag::sch::pin_object(std::move(inst), std::move(attr)), sig};
    } else if (p->getType() == oa::oacRectType) {
        oa::oaRect *r = static_cast<oa::oaRect *>(p);
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
                        (const char *)p->getType().getName()));
    }
}

// Read method for terminals

std::pair<std::string, cbag::sch::pin_figure> OAReader::read_terminal_single(oa::oaTerm *term) {
    // parse terminal name
    oa::oaString term_name_oa;
    term->getName(ns, term_name_oa);

    // get pin
    oa::oaIter<oa::oaPin> pin_iter(term->getPins());
    oa::oaPin *pin_ptr = pin_iter.getNext();
    if (pin_ptr == nullptr) {
        throw std::invalid_argument(
            fmt::format("Terminal {} has no pins.", (const char *)term_name_oa));
    }
    if (pin_iter.getNext() != nullptr) {
        throw std::invalid_argument(
            fmt::format("Terminal {} has more than one pin.", (const char *)term_name_oa));
    }

    // get pin figure
    oa::oaIter<oa::oaPinFig> fig_iter(pin_ptr->getFigs());
    oa::oaPinFig *fig_ptr = fig_iter.getNext();
    if (fig_ptr == nullptr) {
        throw std::invalid_argument(
            fmt::format("Terminal {} has no figures.", (const char *)term_name_oa));
    }
    if (fig_iter.getNext() != nullptr) {
        throw std::invalid_argument(
            fmt::format("Terminal {} has more than one figures.", (const char *)term_name_oa));
    }

    return {std::string(term_name_oa), read_pin_figure(term, fig_ptr)};
};

// Read method for schematic/symbol cell view

cbag::sch::cellview OAReader::read_sch_cellview(oa::oaDesign *p) {
    oa::oaBlock *block = p->getTopBlock();
    oa::oaString lib_oa;
    oa::oaString cell_oa;
    oa::oaString tmp;
    oa::oaBox bbox;
    p->getLibName(ns, lib_oa);
    p->getCellName(ns, cell_oa);
    p->getViewName(ns, tmp);
    block->getBBox(bbox);

    cbag::sch::cellview ans(lib_oa, cell_oa, tmp, bbox);

    // read terminals
    logger->info("Reading terminals");
    oa::oaIter<oa::oaTerm> term_iter(block->getTerms());
    oa::oaTerm *term_ptr;
    while ((term_ptr = term_iter.getNext()) != nullptr) {
        term_ptr->getName(ns, tmp);
        switch (term_ptr->getTermType()) {
        case oa::oacInputTermType:
            ans.in_terms.insert(read_terminal_single(term_ptr));
            break;
        case oa::oacOutputTermType:
            ans.out_terms.insert(read_terminal_single(term_ptr));
            break;
        case oa::oacInputOutputTermType:
            ans.io_terms.insert(read_terminal_single(term_ptr));
            break;
        default:
            throw std::invalid_argument(
                fmt::format("Terminal {} has invalid type: {}", (const char *)tmp,
                            (const char *)term_ptr->getTermType().getName()));
        }
    }

    // read shapes
    logger->info("Reading shapes");
    oa::oaIter<oa::oaShape> shape_iter(block->getShapes());
    oa::oaShape *shape_ptr;
    while ((shape_ptr = shape_iter.getNext()) != nullptr) {
        logger->info("shape type: {}", (const char *)shape_ptr->getType().getName());
        // skip shapes associated with pins.  We got those already.
        if (include_shape(shape_ptr)) {
            ans.shapes.push_back(read_shape(shape_ptr));
        } else {
            logger->info("Skipping this shape");
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
        ans.props.insert(read_prop(prop_ptr));
    }
    logger->info("properties end");

    logger->info("Reading AppDefs");
    oa::oaIter<oa::oaAppDef> appdef_iter(p->getAppDefs());
    oa::oaAppDef *appdef_ptr;
    while ((appdef_ptr = appdef_iter.getNext()) != nullptr) {
        ans.app_defs.insert(read_app_def(p, appdef_ptr));
    }
    logger->info("AppDefs end");

    /*
    logger->info("Reading design groups");
    oa::oaIter<oa::oaGroup> grp_iter(p->getGroups(oacGroupIterBlockDomain | oacGroupIterModDomain |
                                                  oacGroupIterNoDomain | oacGroupIterOccDomain));
    oa::oaGroup *grp_ptr;
    while ((grp_ptr = grp_iter.getNext()) != nullptr) {
        print_group(grp_ptr);
    }
    logger->info("Groups end");
    */

    /*
    logger->info("Reading cell DM data");
    oa::oaScalarName lib_name;
    oa::oaScalarName cell_name;
    p->getLibName(lib_name);
    p->getCellName(cell_name);
    oa::oaCellDMData *data = oa::oaCellDMData::open(lib_name, cell_name, 'r');
    print_dm_data(data);

    logger->info("Reading cellview DM data");
    oa::oaScalarName view_name;
    p->getViewName(view_name);
    if (oa::oaCellViewDMData::exists(lib_name, cell_name, view_name)) {
        oa::oaCellViewDMData *cv_data =
            oa::oaCellViewDMData::open(lib_name, cell_name, view_name, 'r');
        print_dm_data(cv_data);
    }

    logger->info("Reading time stamps");
    for (unsigned int idx = 0; idx < 81; ++idx) {
        try {
            oa::oaDesignDataType data_type(static_cast<oa::oaDesignDataTypeEnum>(idx));
            logger->info("{} timestamp = {}", (const char *)data_type.getName(),
                         p->getTimeStamp(data_type));
        } catch (...) {
            logger->info("error on idx = {}", idx);
        }
    }
    */

    logger->info("Finish reading schematic/symbol cellview");
    return ans;
}

void OAReader::print_prop(oa::oaObject *obj) {
    if (obj->hasProp()) {
        oa::oaIter<oa::oaProp> prop_iter(obj->getProps());
        oa::oaProp *p;
        logger->info("Reading properties");
        while ((p = prop_iter.getNext()) != nullptr) {
            oa::oaString name;
            oa::oaString val;
            p->getName(name);
            p->getValue(val);
            logger->info("Property name = {}, value = {}, type = {}", (const char *)name,
                         (const char *)val, (const char *)p->getType().getName());
            if (val == "oaHierProp") {
                logger->info("Hierarchical properties:");
                print_prop(p);
            } else if (p->getType().getName() == "AppProp") {
                static_cast<oa::oaAppProp *>(p)->getAppType(val);
                logger->info("AppProp type: {}", (const char *)val);
            }
        }
        logger->info("properties end");
    } else {
        logger->info("No properties");
    }
}

void OAReader::print_app_def(oa::oaDesign *dsn, oa::oaAppDef *p) {
    oa::oaString name;
    p->getName(name);
    // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
    switch (p->getType()) {
    case oa::oacIntAppDefType: {
        logger->info("AppDef name: {}, AppDef value: {}", (const char *)name,
                     (static_cast<oa::oaIntAppDef<oa::oaDesign> *>(p))->get(dsn));
        break;
    }
    case oa::oacStringAppDefType: {
        oa::oaString val;
        (static_cast<oa::oaStringAppDef<oa::oaDesign> *>(p))->get(dsn, val);
        logger->info("AppDef name: {}, AppDef value: {}", (const char *)name, (const char *)val);
        break;
    }
    default: {
        throw std::invalid_argument(
            fmt::format("Unsupported OA AppDef {} with type: {}, see developer.",
                        (const char *)name, (const char *)p->getType().getName()));
    }
    }
    print_prop(p);
}

void OAReader::print_group(oa::oaGroup *p) {
    oa::oaString grp_str;
    p->getName(grp_str);
    logger->info("group name: {}, domain: {}", (const char *)grp_str,
                 (const char *)p->getGroupDomain().getName());
    logger->info("group has prop: {}, has appdef: {}", p->hasProp(), p->hasAppDef());
    p->getDef()->getName(grp_str);
    logger->info("group def name: {}", (const char *)grp_str);
    oa::oaIter<oa::oaGroupMember> mem_iter(p->getMembers());
    oa::oaGroupMember *mem_ptr;
    while ((mem_ptr = mem_iter.getNext()) != nullptr) {
        logger->info("group object type: {}",
                     (const char *)mem_ptr->getObject()->getType().getName());
    }
}

void OAReader::print_dm_data(oa::oaDMData *data) {
    logger->info("Has app def: {}", data->hasAppDef());
    print_prop(data);
    logger->info("Reading groups");
    oa::oaIter<oa::oaGroup> grp_iter(data->getGroups());
    oa::oaGroup *grp_ptr;
    while ((grp_ptr = grp_iter.getNext()) != nullptr) {
        print_group(grp_ptr);
    }
    logger->info("Groups end");

    logger->info("Reading AppObjects");
    oa::oaIter<oa::oaAppObjectDef> odef_iter(data->getAppObjectDefs());
    oa::oaAppObjectDef *odef_ptr;
    while ((odef_ptr = odef_iter.getNext()) != nullptr) {
        logger->info("has object def");
    }
    logger->info("AppObjects end");

    logger->info("Reading time stamps");
    for (unsigned int idx = 0; idx < 15; ++idx) {
        try {
            oa::oaDMDataType data_type(static_cast<oa::oaDMDataTypeEnum>(idx));
            logger->info("{} timestamp = {}", (const char *)data_type.getName(),
                         data->getTimeStamp(data_type));
        } catch (...) {
            logger->info("error on idx = {}", idx);
        }
    }
}

} // namespace cbagoa
