/** \file write_oa.cpp
 *  \brief This file writes CBAG data structure to OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#include <utility>
#include <ctime>

#include <boost/variant.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <easylogging++.h>

#include <cbag/spirit/ast.h>

#include <cbagoa/write_oa.h>
#include <cbagoa/name.h>


namespace bsp = cbag::spirit;
namespace bsa = cbag::spirit::ast;

namespace cbagoa {

    class make_pin_fig_visitor : public boost::static_visitor<> {
    public:
        make_pin_fig_visitor(const oa::oaCdbaNS *ns, oa::oaBlock *block, oa::oaPin *pin,
                             oa::oaTerm *term, int *cnt)
                : ns(ns), block(block), pin(pin), term(term), cnt(cnt) {}

        void operator()(const cbag::Rect &operand) const {
            oa::oaRect *rect = oa::oaRect::create(block, operand.layer, operand.purpose,
                                                  operand.bbox);
            rect->addToPin(pin);
        }

        void operator()(const cbag::SchPinObject &obj) const {
            oa::oaScalarName lib(*ns, obj.inst.lib_name.c_str());
            oa::oaScalarName cell(*ns, obj.inst.cell_name.c_str());
            oa::oaScalarName view(*ns, obj.inst.view_name.c_str());
            oa::oaScalarName name(*ns, fmt::format("P__{}", (*cnt)++).c_str());

            oa::oaScalarInst *inst = oa::oaScalarInst::create(block, lib, cell, view,
                                                              name, obj.inst.xform);
            inst->addToPin(pin);

            oa::oaAttrDisplay::create(term, obj.attr.attr_type, obj.attr.layer, obj.attr.purpose,
                                      obj.attr.origin, obj.attr.alignment, obj.attr.orient,
                                      obj.attr.font, obj.attr.height, obj.attr.format,
                                      obj.attr.overbar, obj.attr.visible, obj.attr.drafting);
        }

    private:
        const oa::oaCdbaNS *ns;
        oa::oaBlock *block;
        oa::oaPin *pin;
        oa::oaTerm *term;
        int *cnt;
    };

    class make_shape_visitor : public boost::static_visitor<> {
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
            oa::oaShape *p = oa::oaArc::create(block, v.layer, v.purpose, v.bbox, v.ang_start,
                                               v.ang_stop);
            add_shape_to_net(p, v.net);
        }

        void operator()(const cbag::Donut &v) const {
            oa::oaShape *p = oa::oaDonut::create(block, v.layer, v.purpose, v.center, v.radius,
                                                 v.hole_radius);
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
            oa::oaShape *p = oa::oaPath::create(block, v.layer, v.purpose, v.width, v.points,
                                                v.style, v.begin_ext, v.end_ext);
            add_shape_to_net(p, v.net);
        }

        void operator()(const cbag::Text &v) const {
            oa::oaShape *p = oa::oaText::create(block, v.layer, v.purpose, v.text.c_str(),
                                                v.origin, v.alignment, v.orient, v.font, v.height,
                                                v.overbar, v.visible, v.drafting);
            add_shape_to_net(p, v.net);
        }

        void operator()(const cbag::EvalText &v) const {
            oa::oaShape *p = oa::oaEvalText::create(block, v.layer, v.purpose, v.text.c_str(),
                                                    v.origin, v.alignment, v.orient, v.font,
                                                    v.height, v.evaluator.c_str(), v.overbar,
                                                    v.visible, v.drafting);
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

    class make_prop_visitor : public boost::static_visitor<> {
    public:
        explicit make_prop_visitor(oa::oaObject *obj, const std::string &name)
                : obj(obj), name(name.c_str()) {}

        void operator()(const int32_t &v) const {
            oa::oaIntProp::create(obj, name, v);
        }

        void operator()(const double &v) const {
            oa::oaDoubleProp::create(obj, name, v);
        }

        void operator()(const std::string &v) const {
            oa::oaStringProp::create(obj, name, v.c_str());
        }

        void operator()(const cbag::Time &v) const {
            oa::oaTimeProp::create(obj, name, v.time_val);
        }

        void operator()(const cbag::Binary &v) const {
            oa::oaAppProp::create(obj, name, v.name.c_str(),
                                  oa::oaByteArray(
                                          reinterpret_cast<const oa::oaByte *>(v.bin_val.data()),
                                          static_cast<oa::oaUInt4>(v.bin_val.size())));
        }

    private:
        oa::oaObject *obj;
        oa::oaString name;
    };

    void OAWriter::create_terminal_pin(oa::oaBlock *block, int &pin_cnt,
                                       const std::map<bsa::name, cbag::PinFigure> &map,
                                       oa::oaTermTypeEnum term_type) {
        oa::oaName term_name;
        for (auto const &pair : map) {
            // create terminal, net, and pin
            std::string key = to_string(pair.first);
            LOG(DEBUG) << "Creating terminal: " << key;
            term_name.init(ns, key.c_str());
            LOG(DEBUG) << "Creating terminal net";
            oa::oaNet *term_net = oa::oaNet::find(block, term_name);
            if (term_net == nullptr || term_net->isImplicit()) {
                term_net = oa::oaNet::create(block, term_name, pair.second.sig_type);
            }
            LOG(DEBUG) << "Creating terminal";
            oa::oaTerm *term = oa::oaTerm::create(term_net, term_name, term_type);
            LOG(DEBUG) << "Creating terminal pin";
            oa::oaPin *pin = oa::oaPin::create(term);

            LOG(DEBUG) << "Creating terminal shape";
            boost::apply_visitor(make_pin_fig_visitor(&ns, block, pin, term, &pin_cnt),
                                 pair.second.obj);
        }
    }

    void OAWriter::write_sch_cellview(const cbag::SchCellView &cv, oa::oaDesign *dsn) {
        oa::oaBlock *block = oa::oaBlock::create(dsn);

        int pin_cnt = 0;
        LOG(INFO) << "Writing input terminals";
        create_terminal_pin(block, pin_cnt, cv.in_terms, oa::oacInputTermType);
        LOG(INFO) << "Writing output terminals";
        create_terminal_pin(block, pin_cnt, cv.out_terms, oa::oacOutputTermType);
        LOG(INFO) << "Writing inout terminals";
        create_terminal_pin(block, pin_cnt, cv.io_terms, oa::oacInputOutputTermType);

        LOG(INFO) << "Writing shapes";
        make_shape_visitor shape_visitor(block, &ns);
        for (auto const &shape : cv.shapes) {
            boost::apply_visitor(shape_visitor, shape);
        }

        LOG(INFO) << "Writing instances";
        oa::oaScalarName lib, cell, view, name;
        oa::oaName term_name, net_name;
        for (auto const &pair : cv.instances) {
            std::string inst_name = to_string(pair.first);
            lib.init(ns, pair.second.lib_name.c_str());
            cell.init(ns, pair.second.cell_name.c_str());
            view.init(ns, pair.second.view_name.c_str());
            name.init(ns, pair.first.base.c_str());
            oa::oaInst *ptr;
            if (pair.first.is_vector()) {
                ptr = oa::oaVectorInst::create(block, lib, cell, view, name,
                                               pair.first.idx_range.start,
                                               pair.first.idx_range.stop, pair.second.xform);
            } else {
                ptr = oa::oaScalarInst::create(block, lib, cell, view, name, pair.second.xform);
            }
            oa::oaNetTermNameArray conn_data(
                    static_cast<oa::oaUInt4>(pair.second.connections.size()));
            for (auto const &term_net_pair : pair.second.connections) {
                std::string term_str = to_string(term_net_pair.first);
                std::string net_str = to_string(term_net_pair.second);
                LOG(INFO) << "Connecting inst " << inst_name << " terminal " << term_str
                          << " to " << net_str;
                term_name.init(ns, term_str.c_str());
                net_name.init(ns, net_str.c_str());
                oa::oaNet *term_net = oa::oaNet::find(block, net_name);
                if (term_net == nullptr || term_net->isImplicit()) {
                    term_net = oa::oaNet::create(block, net_name);
                }
                conn_data.append(term_net, term_name);
            }
            oa::oaInstTerm::create(ptr, conn_data);

            for (auto const &prop_pair : pair.second.params) {
                boost::apply_visitor(make_prop_visitor(ptr, prop_pair.first), prop_pair.second);
            }
        }

        dsn->save();

        LOG(INFO) << "Writing properties";
        for (auto const &prop_pair : cv.params) {
            if (prop_pair.first == "lastSchematicExtraction") {
                oa::oaTimeProp::create(dsn, "lastSchematicExtraction", std::time(nullptr));
            } else {
                boost::apply_visitor(make_prop_visitor(dsn, prop_pair.first), prop_pair.second);
            }

        }

        LOG(INFO) << "Finish writing schematic/symbol cellview";
    }
}
