/** \file write_oa.cpp
 *  \brief This file writes CBAG data structure to OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#include <ctime>
#include <tuple>
#include <utility>
#include <variant>

#include <boost/range/join.hpp>

#include <cbag/logging/spdlog.h>

#include <cbag/util/sorted_map.h>
#include <cbag/util/sorted_vector.h>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/instance.h>
#include <cbag/layout/via.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/instance.h>
#include <cbag/spirit/ast.h>
#include <cbag/util/name_convert.h>

#include <cbag/oa/oa_polygon.h>
#include <cbag/oa/oa_util.h>
#include <cbag/oa/oa_write.h>

namespace cbagoa {

// TODO: find ways to not hard code these values
constexpr oa::oaLayerNum sch_conn_layer = 228;
constexpr oa::oaPurposeNum sch_conn_purpose = 4294967295;
constexpr oa::oaPurposeNum sch_net_purpose = 237;
constexpr oa::oaCoord sch_stub_len2 = 5;
constexpr auto sch_net_align = oa::oacCenterCenterTextAlign;
constexpr auto sch_net_orient = oa::oacR0;
constexpr auto sch_net_font = oa::oacStickFont;
constexpr auto sch_net_height = 10;

constexpr auto prop_app_type = "ILList";
constexpr auto cell_data_name = "cdfData";
constexpr auto sch_data_parent_name = "dependency";
constexpr auto sch_data_name = "children";
// clang-format off
constexpr auto cell_data = "(promptWidth nil "
                            "fieldHeight nil "
                            "fieldWidth nil "
                            "buttonFieldWidth nil "
                            "formInitProc nil "
                            "doneProc nil "
                            "parameters nil "
                            "propList (modelLabelSet \"\" "
                                      "opPointLabelSet \"\" "
                                      "paramLabelSet \"\" "
                                      "simInfo (nil "
                                                "auLvs (nil "
                                                       "namePrefix \"X\" "
                                                       "termOrder {0} "
                                                       "componentName \"subcircuit\" "
                                                       "netlistProcedure ansLvsCompPrim) "
                                                "auCdl (nil "
                                                       "namePrefix \"X\" "
                                                       "termOrder {0} "
                                                       "componentName \"subcircuit\" "
                                                       "netlistProcedure ansCdlSubcktCall) "
                                                "spectre (nil "
                                                         "termOrder {0} "
                                                         "componentName \"subcircuit\" "
                                                         "netlistProcedure nil) "
                                                "hspiceD (nil "
                                                         "namePrefix \"X\" "
                                                         "termOrder {0} "
                                                         "componentName \"subcircuit\" "
                                                         "netlistProcedure nil))))";
// clang-format on

constexpr oa::oaByte pin_dir = oacTop | oacBottom | oacLeft | oacRight;

oa::oaBox get_box(const cbag::box_t &cbag_obj) {
    return {xl(cbag_obj), yl(cbag_obj), xh(cbag_obj), yh(cbag_obj)};
}

oa::oaPoint get_point(const cbag::point &cbag_obj) { return {cbag_obj.first, cbag_obj.second}; }

oa::oaOrient get_orient(cbag::orientation cbag_enum) {
    switch (cbag_enum) {
    case cbag::oR0:
        return oa::oacR0;
    case cbag::oR90:
        return oa::oacR90;
    case cbag::oR180:
        return oa::oacR180;
    case cbag::oR270:
        return oa::oacR270;
    case cbag::oMY:
        return oa::oacMY;
    case cbag::oMYR90:
        return oa::oacMYR90;
    case cbag::oMX:
        return oa::oacMX;
    case cbag::oMXR90:
        return oa::oacMXR90;
    default:
        throw std::invalid_argument("Unknown cbag orientation code.");
    }
}

oa::oaPointArray get_point_arr(const cbag::point_array &pt_arr) {
    oa::oaPointArray ans(pt_arr.size());
    for (const auto &p : pt_arr) {
        ans.append(oa::oaPoint(p.first, p.second));
    }
    return ans;
}

oa::oaTransform get_xform(const cbag::transformation &xform) {
    return {cbag::x(xform), cbag::y(xform), get_orient(cbag::orient(xform))};
}

oa::oaTermAttrType get_attr_type(cbag::term_attr_type cbag_enum) {
    return static_cast<oa::oaTermAttrTypeEnum>(cbag_enum);
}

oa::oaTextAlign get_text_align(cbag::text_align cbag_enum) {
    return static_cast<oa::oaTextAlignEnum>(cbag_enum);
}

oa::oaTextDisplayFormat get_text_disp_format(cbag::text_disp_format cbag_enum) {
    return static_cast<oa::oaTextDisplayFormatEnum>(cbag_enum);
}

oa::oaFont get_font(cbag::font_t cbag_enum) { return static_cast<oa::oaFontEnum>(cbag_enum); }

oa::oaSigType get_sig_type(cbag::sig_type cbag_enum) {
    return static_cast<oa::oaSigTypeEnum>(cbag_enum);
}

oa::oaTermType get_term_type(cbag::term_type cbag_enum) {
    return static_cast<oa::oaTermTypeEnum>(cbag_enum);
}

oa::oaBlockageType get_blockage_type(cbag::blockage_type cbag_enum) {
    return static_cast<oa::oaBlockageTypeEnum>(cbag_enum);
}

oa::oaPathStyle get_path_style(cbag::path_style cbag_enum) {
    return static_cast<oa::oaPathStyleEnum>(cbag_enum);
}

oa::oaViaParam get_via_params(const cbag::layout::via_param &params) {
    oa::oaViaParam ans;
    ans.setCutColumns(params.num[0]);
    ans.setCutRows(params.num[1]);
    ans.setCutWidth(params.cut_dim[0]);
    ans.setCutHeight(params.cut_dim[1]);
    ans.setCutSpacing(oa::oaVector(params.cut_spacing[0], params.cut_spacing[1]));
    ans.setLayer1Enc(oa::oaVector(params.lay1_enc[0], params.lay1_enc[1]));
    ans.setLayer2Enc(oa::oaVector(params.lay2_enc[0], params.lay2_enc[1]));
    ans.setLayer1Offset(oa::oaVector(params.lay1_off[0], params.lay1_off[1]));
    ans.setLayer2Offset(oa::oaVector(params.lay2_off[0], params.lay2_off[1]));
    return ans;
}

class make_pin_fig_visitor {
  private:
    const oa::oaCdbaNS &ns;
    oa::oaBlock *block;
    oa::oaPin *pin;
    oa::oaTerm *term;
    int *cnt;

  public:
    make_pin_fig_visitor(const oa::oaCdbaNS &ns, oa::oaBlock *block, oa::oaPin *pin,
                         oa::oaTerm *term, int *cnt)
        : ns(ns), block(block), pin(pin), term(term), cnt(cnt) {}

    void operator()(const cbag::sch::rectangle &operand) const {
        auto rect =
            oa::oaRect::create(block, operand.layer, operand.purpose, get_box(operand.bbox));
        rect->addToPin(pin);
    }

    void operator()(const cbag::sch::pin_object &obj) const {
        oa::oaScalarName lib(ns, obj.inst.lib_name.c_str());
        oa::oaScalarName cell(ns, obj.inst.cell_name.c_str());
        oa::oaScalarName view(ns, obj.inst.view_name.c_str());
        oa::oaScalarName name(ns, fmt::format("P__{}", (*cnt)++).c_str());

        auto inst =
            oa::oaScalarInst::create(block, lib, cell, view, name, get_xform(obj.inst.xform));
        inst->addToPin(pin);

        oa::oaAttrDisplay::create(term, get_attr_type(obj.attr.attr_type), obj.attr.layer,
                                  obj.attr.purpose, get_point(obj.attr.origin),
                                  get_text_align(obj.attr.alignment), get_orient(obj.attr.orient),
                                  get_font(obj.attr.font), obj.attr.height,
                                  get_text_disp_format(obj.attr.format), obj.attr.overbar,
                                  obj.attr.visible, obj.attr.drafting);
    }
};

class make_shape_visitor {
  private:
    oa::oaBlock *block;
    const oa::oaCdbaNS &ns;

  public:
    explicit make_shape_visitor(oa::oaBlock *block, const oa::oaCdbaNS &ns)
        : block(block), ns(ns) {}

    void operator()(const cbag::sch::rectangle &v) const {
        auto p = dynamic_cast<oa::oaShape *>(
            oa::oaRect::create(block, v.layer, v.purpose, get_box(v.bbox)));
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::sch::polygon &v) const {
        auto p = dynamic_cast<oa::oaShape *>(
            oa::oaPolygon::create(block, v.layer, v.purpose, get_point_arr(v.points)));
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::sch::arc &v) const {
        auto p = dynamic_cast<oa::oaShape *>(
            oa::oaArc::create(block, v.layer, v.purpose, get_box(v.bbox), v.ang_start, v.ang_stop));
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::sch::donut &v) const {
        auto p = dynamic_cast<oa::oaShape *>(oa::oaDonut::create(
            block, v.layer, v.purpose, get_point(v.center), v.radius, v.hole_radius));
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::sch::ellipse &v) const {
        auto p = dynamic_cast<oa::oaShape *>(
            oa::oaEllipse::create(block, v.layer, v.purpose, get_box(v.bbox)));
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::sch::line &v) const {
        auto p = dynamic_cast<oa::oaShape *>(
            oa::oaLine::create(block, v.layer, v.purpose, get_point_arr(v.points)));
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::sch::path &v) const {
        auto p = dynamic_cast<oa::oaShape *>(
            oa::oaPath::create(block, v.layer, v.purpose, v.width, get_point_arr(v.points),
                               get_path_style(v.style), v.begin_ext, v.end_ext));
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::sch::text_t &v) const {
        auto p = dynamic_cast<oa::oaShape *>(
            oa::oaText::create(block, v.layer, v.purpose, v.text.c_str(), get_point(v.origin),
                               get_text_align(v.alignment), get_orient(v.orient), get_font(v.font),
                               v.height, v.overbar, v.visible, v.drafting));
        add_shape_to_net(p, v.net);
    }

    void operator()(const cbag::sch::eval_text &v) const {
        auto p = dynamic_cast<oa::oaShape *>(oa::oaEvalText::create(
            block, v.layer, v.purpose, v.text.c_str(), get_point(v.origin),
            get_text_align(v.alignment), get_orient(v.orient), get_font(v.font), v.height,
            v.evaluator.c_str(), v.overbar, v.visible, v.drafting));
        add_shape_to_net(p, v.net);
    }

  private:
    void add_shape_to_net(oa::oaShape *ptr, const std::string &net) const {
        if (!net.empty()) {
            oa::oaName net_name(ns, net.c_str());
            auto np = oa::oaNet::find(block, net_name);
            if (np == nullptr || np->isImplicit()) {
                np = oa::oaNet::create(block, net_name);
            }
            ptr->addToNet(np);
        }
    }
};

class make_prop_visitor {
  private:
    oa::oaObject *obj;
    oa::oaString name;

  public:
    explicit make_prop_visitor(oa::oaObject *obj, const std::string &name)
        : obj(obj), name(name.c_str()) {}

    void operator()(const int_fast32_t &v) const { oa::oaIntProp::create(obj, name, v); }

    void operator()(const double_t &v) const { oa::oaDoubleProp::create(obj, name, v); }

    void operator()(const bool &v) const {
        oa::oaBooleanProp::create(obj, name, static_cast<oa::oaBoolean>(v));
    }

    void operator()(const std::string &v) const { oa::oaStringProp::create(obj, name, v.c_str()); }

    void operator()(const cbag::time_struct &v) const {
        oa::oaTimeProp::create(obj, name, v.time_val);
    }

    void operator()(const cbag::binary_t &v) const {
        oa::oaAppProp::create(
            obj, name, v.name.c_str(),
            oa::oaByteArray(reinterpret_cast<const oa::oaByte *>(v.bin_val.data()),
                            static_cast<oa::oaUInt4>(v.bin_val.size())));
    }
};

class make_app_def_visitor {
  private:
    oa::oaDesign *obj;
    oa::oaString name;

  public:
    explicit make_app_def_visitor(oa::oaDesign *obj, const std::string &name)
        : obj(obj), name(name.c_str()) {}

    void operator()(const int_fast32_t &v) const {
        auto ptr = oa::oaIntAppDef<oa::oaDesign>::find(name);
        if (ptr == nullptr) {
            ptr = oa::oaIntAppDef<oa::oaDesign>::get(name);
        }
        ptr->set(obj, v);
    }

    void operator()(const bool &v) const { operator()(static_cast<int_fast32_t>(v)); }

    void operator()(const double_t &v) const {
        throw std::invalid_argument("double AppDef not supported yet.");
    }

    void operator()(const std::string &v) const {
        auto ptr = oa::oaStringAppDef<oa::oaDesign>::find(name);
        if (ptr == nullptr) {
            ptr = oa::oaStringAppDef<oa::oaDesign>::get(name);
        }
        ptr->set(obj, v.c_str());
    }

    void operator()(const cbag::time_struct &v) const {
        throw std::invalid_argument("time AppDef not supported yet.");
    }

    void operator()(const cbag::binary_t &v) const {
        throw std::invalid_argument("binary AppDef not supported yet.");
    }
};

void create_terminal_pin(const oa::oaCdbaNS &ns, spdlog::logger &logger, oa::oaBlock *block,
                         int &pin_cnt,
                         const cbag::util::sorted_map<std::string, cbag::sch::pin_figure> &map) {
    oa::oaName term_name;
    for (auto const &pair : map) {
        // create terminal, net, and pin
        logger.info("Creating terminal {}", pair.first);
        term_name.init(ns, pair.first.c_str());
        logger.info("Creating terminal net");
        auto term_net = oa::oaNet::find(block, term_name);
        if (term_net == nullptr || term_net->isImplicit()) {
            term_net = oa::oaNet::create(block, term_name, get_sig_type(pair.second.stype));
        }
        logger.info("Creating terminal");
        auto term = oa::oaTerm::create(term_net, term_name, get_term_type(pair.second.ttype));
        logger.info("Creating terminal pin");
        auto pin = oa::oaPin::create(term);
        logger.info("Creating terminal shape");
        std::visit(make_pin_fig_visitor(ns, block, pin, term, &pin_cnt), pair.second.obj);
        logger.info("Create terminal done");
    }
}

void write_sch_cell_data(const cbag::sch::cellview &cv, const oa::oaScalarName &lib_name,
                         const oa::oaScalarName &cell_name, const oa::oaScalarName &view_name,
                         const std::string &term_order) {

    // get dependencies
    cbag::util::sorted_vector<std::tuple<std::string, std::string, std::string>> dep_set;
    for (auto const &inst : cv.instances) {
        dep_set.emplace_unique(inst.second->lib_name, inst.second->cell_name,
                               inst.second->view_name);
    }

    // build dependencies
    std::stringstream dependencies;
    auto dep_cursor = dep_set.begin();
    auto dep_end = dep_set.end();
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
    auto cdf_data_str = fmt::format(cell_data, term_order);
    oa::oaByteArray cdf_data(reinterpret_cast<const oa::oaByte *>(cdf_data_str.c_str()),
                             cdf_data_str.size());
    auto data = oa::oaCellDMData::open(lib_name, cell_name, 'w');
    oa::oaAppProp::create(data, cell_data_name, prop_app_type, cdf_data);
    data->save();
    data->close();

    // create cellview data
    std::string cv_cdf_data_str = dependencies.str();
    oa::oaByteArray cv_cdf_data(reinterpret_cast<const oa::oaByte *>(cv_cdf_data_str.c_str()),
                                cv_cdf_data_str.size());
    auto cv_data = oa::oaCellViewDMData::open(lib_name, cell_name, view_name, 'w');
    auto cv_prop_parent = oa::oaHierProp::create(cv_data, sch_data_parent_name);
    oa::oaAppProp::create(cv_prop_parent, sch_data_name, prop_app_type, cv_cdf_data);
    cv_data->save();
    cv_data->close();
}

void write_sch_cellview(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                        spdlog::logger &logger, const std::string &lib_name,
                        const std::string &cell_name, const std::string &view_name, bool is_sch,
                        const cbag::sch::cellview &cv, const str_map_t *rename_map) {

    auto dsn = open_design(ns_native, logger, lib_name, cell_name, view_name, 'w',
                           is_sch ? oa::oacSchematic : oa::oacSchematicSymbol);
    logger.info("Writing cellview {}__{}({})", lib_name, cell_name, view_name);

    oa::oaScalarName dsn_lib;
    dsn->getLibName(dsn_lib);

    oa::oaBlock *block = oa::oaBlock::create(dsn);

    // build term order
    std::stringstream term_order;
    auto cv_info = cv.get_info(cv.cell_name);
    auto tmp1 = boost::range::join(cv_info.in_terms, cv_info.out_terms);
    auto tmp2 = boost::range::join(tmp1, cv_info.io_terms);
    auto cursor = tmp2.begin();
    auto stop = tmp2.end();
    term_order << '(';
    if (cursor != stop) {
        term_order << '"' << *cursor << '"';
        ++cursor;
    }
    for (; cursor != stop; ++cursor) {
        term_order << " \"" << *cursor << '"';
    }
    term_order << ')';
    auto term_order_str = term_order.str();

    int pin_cnt = 0;
    logger.info("Writing terminals");
    create_terminal_pin(ns, logger, block, pin_cnt, cv.terminals);

    // TODO: add shape support for schematic
    if (!is_sch) {
        logger.info("Writing shapes");
        make_shape_visitor shape_visitor(block, ns);
        for (auto const &shape : cv.shapes) {
            std::visit(shape_visitor, shape);
        }
    }

    logger.info("Writing instances");
    oa::oaScalarName lib, cell, view, name;
    oa::oaName term_name, net_name;
    for (auto const &pair : cv.instances) {
        const auto &inst_name = pair.first;
        auto inst = pair.second.get();
        logger.info("Writing instance {}", inst_name);
        auto nu = cbag::util::parse_cdba_name_unit(inst_name);

        auto inst_xform = get_xform(inst->xform);
        if (inst->is_primitive) {
            lib.init(ns, inst->lib_name.c_str());
            cell.init(ns, inst->cell_name.c_str());
        } else {
            lib = dsn_lib;
            cell.init(ns, rename_map->find(inst->cell_name)->second.c_str());
        }

        view.init(ns, inst->view_name.c_str());
        name.init(ns, nu.base.c_str());
        oa::oaInst *ptr;
        if (nu.is_vector()) {
            ptr = oa::oaVectorInst::create(block, lib, cell, view, name, nu.idx_range.start,
                                           nu.idx_range.stop, inst_xform);
        } else {
            ptr = oa::oaScalarInst::create(block, lib, cell, view, name, inst_xform);
        }
        for (auto const &term_net_pair : inst->connections) {
            logger.info("Connecting inst {} terminal {} to {}", inst_name, term_net_pair.first,
                        term_net_pair.second);
            term_name.init(ns, term_net_pair.first.c_str());
            net_name.init(ns, term_net_pair.second.c_str());
            auto term_net = oa::oaNet::find(block, net_name);
            if (term_net == nullptr || term_net->isImplicit()) {
                term_net = oa::oaNet::create(block, net_name);
            }
            auto inst_term = oa::oaInstTerm::create(term_net, ptr, term_name);
            auto term = inst_term->getTerm();
            oa::oaIter<oa::oaPin> pin_iter(term->getPins());
            auto pin = pin_iter.getNext();
            oa::oaIter<oa::oaPinFig> pin_fig_iter(pin->getFigs());
            auto pin_fig = pin_fig_iter.getNext();
            oa::oaBox pin_box;
            oa::oaPoint pts[2];
            pin_fig->getBBox(pin_box);
            pin_box.transform(inst_xform);
            pin_box.getCenter(pts[0]);
            auto x = pts[0].x();
            auto y = pts[0].y();

            // create stub connection
            pts[1] = {x + 2 * sch_stub_len2, y + 2 * sch_stub_len2};
            oa::oaPointArray pt_arr(pts, 2);
            auto line = oa::oaLine::create(block, sch_conn_layer, sch_conn_purpose, pt_arr);
            line->addToNet(term_net);
            oa::oaPoint mid(x + sch_stub_len2, y + sch_stub_len2);
            auto text = oa::oaText::create(block, sch_conn_layer, sch_net_purpose,
                                           term_net_pair.second.c_str(), mid, sch_net_align,
                                           sch_net_orient, sch_net_font, sch_net_height);
            text->addToNet(term_net);
        }

        logger.info("Writing instance {} params", inst_name);
        for (auto const &prop_pair : inst->params) {
            std::visit(make_prop_visitor(ptr, prop_pair.first), prop_pair.second);
        }
        logger.info("Writing instance {} done", inst_name);
    }

    logger.info("Writing properties");
    for (auto const &prop_pair : cv.props) {
        // skip last extraction timestamp
        if (prop_pair.first == "portOrder") {
            const unsigned char *term_order_ptr =
                reinterpret_cast<const unsigned char *>(term_order_str.c_str());
            std::visit(make_prop_visitor(dsn, prop_pair.first),
                       cbag::value_t(cbag::binary_t{
                           prop_app_type,
                           std::string(term_order_ptr, term_order_ptr + term_order_str.size())}));
        } else {
            std::visit(make_prop_visitor(dsn, prop_pair.first), prop_pair.second);
        }
    }

    logger.info("Writing AppDefs");
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
        auto num_op = 2;
        auto timestamp = dsn->getTimeStamp(oa::oacDesignDataType);
        auto pptr = oa::oaProp::find(dsn, "connectivityLastUpdated");
        (static_cast<oa::oaIntProp *>(pptr))->setValue(timestamp + num_op);
        pptr = oa::oaProp::find(dsn, "schGeometryLastUpdated");
        (static_cast<oa::oaIntProp *>(pptr))->setValue(timestamp + num_op);
    }
    dsn->save();
    dsn->close();
    logger.info("Finish writing schematic/symbol cellview");
}

void create_lay_inst(const oa::oaCdbaNS &ns, oa::oaBlock *blk, const std::string &name,
                     const cbag::layout::instance &inst, const char *lib_name,
                     const char *view_name, const str_map_t *rename_map) {
    oa::oaScalarName lib_oa(ns, inst.get_lib_name(lib_name).c_str());
    oa::oaScalarName cell_oa(ns, inst.get_cell_name(rename_map).c_str());
    oa::oaScalarName view_oa(ns, inst.get_view_name(view_name).c_str());
    oa::oaScalarName inst_name(ns, name.c_str());

    // create oa ParamArray
    oa::oaParamArray oa_params;

    auto params_ptr = (oa_params.getNumElements() == 0) ? nullptr : &oa_params;
    auto xform = get_xform(inst.xform);
    if (inst.nx > 1 || inst.ny > 1) {
        // convert BAG array parameters to OA array parameters
        auto [oa_nx, oa_ny, oa_spx, oa_spy] =
            cbag::convert_array(inst.xform, inst.nx, inst.ny, inst.spx, inst.spy);
        oa::oaArrayInst::create(blk, lib_oa, cell_oa, view_oa, inst_name, xform, oa_spx, oa_spy,
                                oa_ny, oa_nx, params_ptr);
    } else {
        oa::oaScalarInst::create(blk, lib_oa, cell_oa, view_oa, inst_name, xform, params_ptr);
    }
}

void set_point_array(const cbag::layout::polygon &poly, oa::oaPointArray &arr) {
    auto size = poly.size();
    arr.setSize(size);
    auto start = poly.begin();
    for (std::size_t idx = 0; idx < size; ++start, ++idx) {
        arr[idx] = oa::oaPoint(start->x(), start->y());
    }
    arr.setNumElements(size);
}

void create_lay_polygon(oa::oaBlock *blk, cbag::lay_t layer, cbag::purp_t purpose,
                        const cbag::layout::polygon &poly, oa::oaPointArray &arr) {
    set_point_array(poly, arr);
    oa::oaPolygon::create(blk, layer, purpose, arr);
}

void create_lay_geometry(spdlog::logger &logger, oa::oaBlock *blk, cbag::lay_t layer,
                         cbag::purp_t purpose, const cbag::layout::geometry &geo) {
    polygon_writer w(blk, layer, purpose, logger);
    geo.write_geometry(w);
    w.record_last();
}

void create_lay_via(spdlog::logger &logger, oa::oaBlock *blk, oa::oaTech *tech,
                    const cbag::layout::via &v) {
    auto &via_id = v.get_via_id();
    auto &v_params = v.get_params();
    auto via_def = static_cast<oa::oaStdViaDef *>(oa::oaViaDef::find(tech, via_id.c_str()));
    if (via_def == nullptr) {
        logger.warn("unknown via ID {}, skipping.", via_id);
        return;
    }

    auto via_params = get_via_params(v_params);
    oa::oaStdVia::create(blk, via_def, get_xform(v.xform), &via_params);
}

void create_lay_pin(spdlog::logger &logger, const oa::oaCdbaNS &ns, oa::oaBlock *blk,
                    cbag::lay_t lay, cbag::purp_t purp, const cbag::layout::pin &pin,
                    bool make_pin_obj) {
    if (!is_physical(pin)) {
        logger.warn("non-physical bbox {} on pin layer ({}, {}), skipping.", to_string(pin), lay,
                    purp);
        return;
    }
    oa::oaPoint center(xm(pin), ym(pin));
    auto w = width(pin);
    auto text_h = height(pin);
    oa::oaOrient orient(oa::oacR0);
    if (text_h > w) {
        orient = oa::oacR90;
        text_h = w;
    }

    oa::oaText::create(blk, lay, purp, pin.label.c_str(), center, oa::oacCenterCenterTextAlign,
                       orient, oa::oacRomanFont, text_h);
    if (make_pin_obj) {
        auto r = oa::oaRect::create(blk, lay, purp, oa::oaBox(xl(pin), yl(pin), xh(pin), yh(pin)));

        oa::oaName term_name(ns, pin.net.c_str());
        auto term = oa::oaTerm::find(blk, term_name);
        if (term == nullptr) {
            auto net = oa::oaNet::find(blk, term_name);
            if (net == nullptr) {
                net = oa::oaNet::create(blk, term_name);
            }
            term = oa::oaTerm::create(net, term_name);
        }
        auto pin = oa::oaPin::create(term, pin_dir);
        r->addToPin(pin);
    }
}

void write_lay_cellview(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                        spdlog::logger &logger, const std::string &lib_name,
                        const std::string &cell_name, const std::string &view_name,
                        const cbag::layout::cellview &cv, oa::oaTech *tech,
                        const str_map_t *rename_map) {

    auto dsn =
        open_design(ns_native, logger, lib_name, cell_name, view_name, 'w', oa::oacMaskLayout);
    logger.info("Writing cellview {}__{}({})", lib_name, cell_name, view_name);

    // create top block
    auto blk = oa::oaBlock::create(dsn);

    logger.info("Export layout instances.");
    for (auto iter = cv.begin_inst(); iter != cv.end_inst(); ++iter) {
        auto &[inst_name, inst] = *iter;
        create_lay_inst(ns, blk, inst_name, inst, lib_name.c_str(), view_name.c_str(), rename_map);
    }

    logger.info("Export layout geometries.");
    for (auto iter = cv.begin_geometry(); iter != cv.end_geometry(); ++iter) {
        auto &[layer_key, geo] = *iter;
        create_lay_geometry(logger, blk, layer_key.first, layer_key.second, geo);
    }

    logger.info("Export layout vias.");
    for (auto iter = cv.begin_via(); iter != cv.end_via(); ++iter) {
        create_lay_via(logger, blk, tech, *iter);
    }

    logger.info("Export layout blockages.");
    oa::oaPointArray pt_arr;
    for (auto iter = cv.begin_lay_block(); iter != cv.end_lay_block(); ++iter) {
        auto const &block_pair = *iter;
        for (auto const &block : block_pair.second) {
            set_point_array(block, pt_arr);
            oa::oaLayerBlockage::create(blk, get_blockage_type(block.get_type()), block_pair.first,
                                        pt_arr);
        }
    }

    logger.info("Export layout area blockages.");
    for (auto iter = cv.begin_area_block(); iter != cv.end_area_block(); ++iter) {
        set_point_array(*iter, pt_arr);
        oa::oaAreaBlockage::create(blk, pt_arr);
    }

    logger.info("Export layout boundaries.");
    for (auto iter = cv.begin_boundary(); iter != cv.end_boundary(); ++iter) {
        set_point_array(*iter, pt_arr);
        switch (iter->get_type()) {
        case cbag::PR:
            oa::oaPRBoundary::create(blk, pt_arr);
        case cbag::snap:
            oa::oaSnapBoundary::create(blk, pt_arr);
        }
    }

    logger.info("Export layout pins.");
    auto tech_ptr = cv.get_tech();
    auto purp = tech_ptr->get_pin_purpose();
    auto make_pin_obj = tech_ptr->get_make_pin();
    for (auto iter = cv.begin_pin(); iter != cv.end_pin(); ++iter) {
        auto &[lay, pin_list] = *iter;
        for (auto const &pin : pin_list) {
            create_lay_pin(logger, ns, blk, lay, purp, pin, make_pin_obj);
        }
    }

    // save
    dsn->save();
    dsn->close();
} // namespace cbagoa

} // namespace cbagoa
