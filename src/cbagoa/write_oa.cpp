/** \file write_oa.cpp
 *  \brief This file writes CBAG data structure to OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#include <ctime>
#include <set>
#include <tuple>
#include <utility>
#include <variant>

#include <boost/range/join.hpp>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <cbag/cbag.h>
#include <cbag/layout/geometries.h>
#include <cbag/netlist/name_convert.h>
#include <cbagoa/write_oa.h>

namespace cbagoa {

class make_pin_fig_visitor {
  public:
    make_pin_fig_visitor(const oa::oaCdbaNS *ns, oa::oaBlock *block, oa::oaPin *pin,
                         oa::oaTerm *term, int *cnt)
        : ns(ns), block(block), pin(pin), term(term), cnt(cnt) {}

    void operator()(const cbag::Rect &operand) const {
        oa::oaRect *rect = oa::oaRect::create(block, operand.layer, operand.purpose, operand.bbox);
        rect->addToPin(pin);
    }

    void operator()(const cbag::SchPinObject &obj) const {
        oa::oaScalarName lib(*ns, obj.inst.lib_name.c_str());
        oa::oaScalarName cell(*ns, obj.inst.cell_name.c_str());
        oa::oaScalarName view(*ns, obj.inst.view_name.c_str());
        oa::oaScalarName name(*ns, fmt::format("P__{}", (*cnt)++).c_str());

        oa::oaScalarInst *inst =
            oa::oaScalarInst::create(block, lib, cell, view, name, obj.inst.xform);
        inst->addToPin(pin);

        oa::oaAttrDisplay::create(term, obj.attr.attr_type, obj.attr.layer, obj.attr.purpose,
                                  obj.attr.origin, obj.attr.alignment, obj.attr.orient,
                                  obj.attr.font, obj.attr.height, obj.attr.format, obj.attr.overbar,
                                  obj.attr.visible, obj.attr.drafting);
    }

  private:
    const oa::oaCdbaNS *ns;
    oa::oaBlock *block;
    oa::oaPin *pin;
    oa::oaTerm *term;
    int *cnt;
};

class make_shape_visitor {
  public:
    explicit make_shape_visitor(oa::oaBlock *block, const oa::oaCdbaNS *ns)
        : block(block), ns(ns) {}

    void operator()(const cbag::Rect &v) const {
        oa::oaShape *p = oa::oaRect::create(block, v.layer, v.purpose, v.bbox);
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::Poly &v) const {
        oa::oaShape *p = oa::oaPolygon::create(block, v.layer, v.purpose, v.points);
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::Arc &v) const {
        oa::oaShape *p =
            oa::oaArc::create(block, v.layer, v.purpose, v.bbox, v.ang_start, v.ang_stop);
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::Donut &v) const {
        oa::oaShape *p =
            oa::oaDonut::create(block, v.layer, v.purpose, v.center, v.radius, v.hole_radius);
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::Ellipse &v) const {
        oa::oaShape *p = oa::oaEllipse::create(block, v.layer, v.purpose, v.bbox);
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::Line &v) const {
        oa::oaShape *p = oa::oaLine::create(block, v.layer, v.purpose, v.points);
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::Path &v) const {
        oa::oaShape *p = oa::oaPath::create(block, v.layer, v.purpose, v.width, v.points, v.style,
                                            v.begin_ext, v.end_ext);
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::Text &v) const {
        oa::oaShape *p =
            oa::oaText::create(block, v.layer, v.purpose, v.text.c_str(), v.origin, v.alignment,
                               v.orient, v.font, v.height, v.overbar, v.visible, v.drafting);
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::EvalText &v) const {
        oa::oaShape *p = oa::oaEvalText::create(
            block, v.layer, v.purpose, v.text.c_str(), v.origin, v.alignment, v.orient, v.font,
            v.height, v.evaluator.c_str(), v.overbar, v.visible, v.drafting);
        add_shape_to_net(p, v.net);
    }

  private:
    void add_shape_to_net(oa::oaShape *ptr, const std::string &net) const {
        if (!net.empty()) {
            oa::oaName net_name(*ns, net.c_str());
            oa::oaNet *np = oa::oaNet::find(block, net_name);
            if (np == nullptr || np->isImplicit()) {
                np = oa::oaNet::create(block, net_name);
            }
            ptr->addToNet(np);
        }
    }

    oa::oaBlock *block;
    const oa::oaCdbaNS *ns;
};

class make_prop_visitor {
  public:
    explicit make_prop_visitor(oa::oaObject *obj, const std::string &name)
        : obj(obj), name(name.c_str()) {}

    void operator()(const int32_t &v) const { oa::oaIntProp::create(obj, name, v); }

    void operator()(const double &v) const { oa::oaDoubleProp::create(obj, name, v); }

    void operator()(const bool &v) const {
        oa::oaBooleanProp::create(obj, name, static_cast<oa::oaBoolean>(v));
    }

    void operator()(const std::string &v) const { oa::oaStringProp::create(obj, name, v.c_str()); }

    void operator()(const cbag::Time &v) const { oa::oaTimeProp::create(obj, name, v.time_val); }

    void operator()(const cbag::Binary &v) const {
        oa::oaAppProp::create(
            obj, name, v.name.c_str(),
            oa::oaByteArray(reinterpret_cast<const oa::oaByte *>(v.bin_val.data()),
                            static_cast<oa::oaUInt4>(v.bin_val.size())));
    }

  private:
    oa::oaObject *obj;
    oa::oaString name;
};

class make_app_def_visitor {
  public:
    explicit make_app_def_visitor(oa::oaDesign *obj, const std::string &name)
        : obj(obj), name(name.c_str()) {}

    void operator()(const int32_t &v) const {
        auto ptr = oa::oaIntAppDef<oa::oaDesign>::find(name);
        if (ptr == nullptr) {
            ptr = oa::oaIntAppDef<oa::oaDesign>::get(name);
        }
        ptr->set(obj, v);
    }

    void operator()(const double &v) const {
        throw std::invalid_argument("double AppDef not supported yet.");
    }

    void operator()(const std::string &v) const {
        auto ptr = oa::oaStringAppDef<oa::oaDesign>::find(name);
        if (ptr == nullptr) {
            ptr = oa::oaStringAppDef<oa::oaDesign>::get(name);
        }
        ptr->set(obj, v.c_str());
    }

    void operator()(const cbag::Time &v) const {
        throw std::invalid_argument("time AppDef not supported yet.");
    }

    void operator()(const cbag::Binary &v) const {
        throw std::invalid_argument("binary AppDef not supported yet.");
    }

  private:
    oa::oaDesign *obj;
    oa::oaString name;
};

void OAWriter::create_terminal_pin(oa::oaBlock *block, int &pin_cnt,
                                   const std::map<std::string, cbag::PinFigure> &map,
                                   oa::oaTermTypeEnum term_type) {
    oa::oaName term_name;
    for (auto const &pair : map) {
        // create terminal, net, and pin
        logger->info("Creating terminal {}", pair.first);
        term_name.init(ns, pair.first.c_str());
        logger->info("Creating terminal net");
        oa::oaNet *term_net = oa::oaNet::find(block, term_name);
        if (term_net == nullptr || term_net->isImplicit()) {
            term_net = oa::oaNet::create(block, term_name, pair.second.sig_type);
        }
        logger->info("Creating terminal");
        oa::oaTerm *term = oa::oaTerm::create(term_net, term_name, term_type);
        logger->info("Creating terminal pin");
        oa::oaPin *pin = oa::oaPin::create(term);
        logger->info("Creating terminal shape");
        std::visit(make_pin_fig_visitor(&ns, block, pin, term, &pin_cnt), pair.second.obj);
        logger->info("Create terminal done");
    }
}

void OAWriter::write_sch_cellview(const cbag::SchCellView &cv, oa::oaDesign *dsn, bool is_sch,
                                  const str_map_t *rename_map) {
    oa::oaScalarName dsn_lib;
    dsn->getLibName(dsn_lib);

    oa::oaBlock *block = oa::oaBlock::create(dsn);

    // build term order
    std::stringstream term_order;
    auto tmp1 = boost::join(cv.in_terms, cv.out_terms);
    auto tmp2 = boost::join(tmp1, cv.io_terms);
    auto cursor = tmp2.begin();
    auto stop = tmp2.end();
    term_order << '(';
    if (cursor != stop) {
        term_order << '"' << (*cursor).first << '"';
        ++cursor;
    }
    for (; cursor != stop; ++cursor) {
        term_order << " \"" << (*cursor).first << '"';
    }
    term_order << ')';
    std::string term_order_str = term_order.str();

    int pin_cnt = 0;
    logger->info("Writing input terminals");
    create_terminal_pin(block, pin_cnt, cv.in_terms, oa::oacInputTermType);
    logger->info("Writing output terminals");
    create_terminal_pin(block, pin_cnt, cv.out_terms, oa::oacOutputTermType);
    logger->info("Writing inout terminals");
    create_terminal_pin(block, pin_cnt, cv.io_terms, oa::oacInputOutputTermType);

    // TODO: add shape support for schematic
    if (!is_sch) {
        logger->info("Writing shapes");
        make_shape_visitor shape_visitor(block, &ns);
        for (auto const &shape : cv.shapes) {
            std::visit(shape_visitor, shape);
        }
    }

    logger->info("Writing instances");
    oa::oaScalarName lib, cell, view, name;
    oa::oaName term_name, net_name;
    for (auto const &pair : cv.instances) {
        logger->info("Writing instance {}", pair.first);
        cbag::spirit::ast::name_unit nu = cbag::parse_cdba_name_unit(pair.first);
        if (pair.second.is_primitive) {
            lib.init(ns, pair.second.lib_name.c_str());
            cell.init(ns, pair.second.cell_name.c_str());
        } else {
            lib = dsn_lib;
            cell.init(ns, rename_map->find(pair.second.cell_name)->second.c_str());
        }

        view.init(ns, pair.second.view_name.c_str());
        name.init(ns, nu.base.c_str());
        oa::oaInst *ptr;
        if (nu.is_vector()) {
            ptr = oa::oaVectorInst::create(block, lib, cell, view, name, nu.idx_range.start,
                                           nu.idx_range.stop, pair.second.xform);
        } else {
            ptr = oa::oaScalarInst::create(block, lib, cell, view, name, pair.second.xform);
        }
        for (auto const &term_net_pair : pair.second.connections) {
            logger->info("Connecting inst {} terminal {} to {}", pair.first, term_net_pair.first,
                         term_net_pair.second);
            term_name.init(ns, term_net_pair.first.c_str());
            net_name.init(ns, term_net_pair.second.c_str());
            oa::oaNet *term_net = oa::oaNet::find(block, net_name);
            if (term_net == nullptr || term_net->isImplicit()) {
                term_net = oa::oaNet::create(block, net_name);
            }
            oa::oaInstTerm *inst_term = oa::oaInstTerm::create(term_net, ptr, term_name);
            oa::oaTerm *term = inst_term->getTerm();
            oa::oaIter<oa::oaPin> pin_iter(term->getPins());
            oa::oaPin *pin = pin_iter.getNext();
            oa::oaIter<oa::oaPinFig> pin_fig_iter(pin->getFigs());
            oa::oaPinFig *pin_fig = pin_fig_iter.getNext();
            oa::oaBox pin_box;
            oa::oaPoint pts[2];
            pin_fig->getBBox(pin_box);
            pin_box.transform(pair.second.xform);
            pin_box.getCenter(pts[0]);
            oa::oaCoord x = pts[0].x();
            oa::oaCoord y = pts[0].y();

            // create stub connection
            pts[1] = {x + 2 * sch_stub_len2, y + 2 * sch_stub_len2};
            oa::oaPointArray pt_arr(pts, 2);
            oa::oaLine *line = oa::oaLine::create(block, sch_conn_layer, sch_conn_purpose, pt_arr);
            line->addToNet(term_net);
            oa::oaPoint mid(x + sch_stub_len2, y + sch_stub_len2);
            oa::oaText *text = oa::oaText::create(block, sch_conn_layer, sch_net_purpose,
                                                  term_net_pair.second.c_str(), mid, sch_net_align,
                                                  sch_net_orient, sch_net_font, sch_net_height);
            text->addToNet(term_net);
        }

        logger->info("Writing instance {} params", pair.first);
        for (auto const &prop_pair : pair.second.params) {
            std::visit(make_prop_visitor(ptr, prop_pair.first), prop_pair.second);
        }
        logger->info("Writing instance {} done", pair.first);
    }

    logger->info("Writing properties");
    for (auto const &prop_pair : cv.props) {
        // skip last extraction timestamp
        if (prop_pair.first == "portOrder") {
            std::visit(
                make_prop_visitor(dsn, prop_pair.first),
                cbag::value_t(cbag::Binary(
                    prop_app_type, reinterpret_cast<const unsigned char *>(term_order_str.c_str()),
                    term_order_str.size())));
        } else {
            std::visit(make_prop_visitor(dsn, prop_pair.first), prop_pair.second);
        }
    }

    logger->info("Writing AppDefs");
    for (auto const &prop_pair : cv.app_defs) {
        std::visit(make_app_def_visitor(dsn, prop_pair.first), prop_pair.second);
    }

    // save
    dsn->save();

    if (is_sch) {
        oa::oaScalarName dsn_cell;
        oa::oaScalarName dsn_view;
        dsn->getCellName(dsn_cell);
        dsn->getViewName(dsn_view);

        // write cellview data
        write_sch_cell_data(cv, dsn_lib, dsn_cell, dsn_view, term_order_str);

        // update extraction timestamp
        uint32_t num_op = 2;
        oa::oaTime timestamp = dsn->getTimeStamp(oa::oacDesignDataType);
        auto pptr = oa::oaProp::find(dsn, "connectivityLastUpdated");
        (static_cast<oa::oaIntProp *>(pptr))->setValue(timestamp + num_op);
        pptr = oa::oaProp::find(dsn, "schGeometryLastUpdated");
        (static_cast<oa::oaIntProp *>(pptr))->setValue(timestamp + num_op);
    }
    dsn->save();

    logger->info("Finish writing schematic/symbol cellview");
}

void OAWriter::write_sch_cell_data(const cbag::SchCellView &cv, const oa::oaScalarName &lib_name,
                                   const oa::oaScalarName &cell_name,
                                   const oa::oaScalarName &view_name,
                                   const std::string &term_order) {

    // get dependencies
    std::set<std::tuple<std::string, std::string, std::string>> dep_set;
    for (auto const &inst : cv.instances) {
        dep_set.emplace(inst.second.lib_name, inst.second.cell_name, inst.second.view_name);
    }

    // build dependencies
    std::stringstream dependencies;
    auto dep_cursor = dep_set.cbegin();
    auto dep_end = dep_set.cend();
    dependencies << '(';
    if (dep_cursor != dep_end) {
        dependencies << "(\"" << std::get<0>(*dep_cursor) << "\" \"" << std::get<1>(*dep_cursor)
                     << "\" \"" << std::get<2>(*dep_cursor) << "\")";
        ++dep_cursor;
    }
    for (; dep_cursor != dep_end; ++dep_cursor) {
        dependencies << " (\"" << std::get<0>(*dep_cursor) << "\" \"" << std::get<1>(*dep_cursor)
                     << "\" \"" << std::get<1>(*dep_cursor) << "\")";
    }
    dependencies << ')';

    // create cell data
    std::string cdf_data_str = fmt::format(cell_data, term_order);
    oa::oaByteArray cdf_data(reinterpret_cast<const oa::oaByte *>(cdf_data_str.c_str()),
                             cdf_data_str.size());
    oa::oaCellDMData *data = oa::oaCellDMData::open(lib_name, cell_name, 'w');
    oa::oaAppProp::create(data, cell_data_name, prop_app_type, cdf_data);
    data->save();
    data->close();

    // create cellview data
    std::string cv_cdf_data_str = dependencies.str();
    oa::oaByteArray cv_cdf_data(reinterpret_cast<const oa::oaByte *>(cv_cdf_data_str.c_str()),
                                cv_cdf_data_str.size());
    oa::oaCellViewDMData *cv_data = oa::oaCellViewDMData::open(lib_name, cell_name, view_name, 'w');
    oa::oaHierProp *cv_prop_parent = oa::oaHierProp::create(cv_data, sch_data_parent_name);
    oa::oaAppProp::create(cv_prop_parent, sch_data_name, prop_app_type, cv_cdf_data);
    cv_data->save();
    cv_data->close();
}

} // namespace cbagoa
