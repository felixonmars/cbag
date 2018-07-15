/** \file write_oa.cpp
 *  \brief This file writes CBAG data structure to OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#include <utility>

#include <boost/variant.hpp>

#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/ostream.h>

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
        explicit make_shape_visitor(oa::oaBlock *block) : block(block) {}

        void operator()(const cbag::Rect &v) const {
            oa::oaRect::create(block, v.layer, v.purpose, v.bbox);
        }

        void operator()(const cbag::Poly &v) const {
            oa::oaPolygon::create(block, v.layer, v.purpose, v.points);
        }

        void operator()(const cbag::Arc &v) const {
            oa::oaArc::create(block, v.layer, v.purpose, v.bbox, v.ang_start, v.ang_stop);
        }

        void operator()(const cbag::Donut &v) const {
            oa::oaDonut::create(block, v.layer, v.purpose, v.center, v.radius, v.hole_radius);
        }

        void operator()(const cbag::Ellipse &v) const {
            oa::oaEllipse::create(block, v.layer, v.purpose, v.bbox);
        }

        void operator()(const cbag::Line &v) const {
            oa::oaLine::create(block, v.layer, v.purpose, v.points);
        }

        void operator()(const cbag::Path &v) const {
            oa::oaPath::create(block, v.layer, v.purpose, v.width, v.points, v.style,
                               v.begin_ext, v.end_ext);
        }

        void operator()(const cbag::Text &v) const {
            oa::oaText::create(block, v.layer, v.purpose, v.text.c_str(),
                               v.origin, v.alignment, v.orient, v.font, v.height, v.overbar,
                               v.visible, v.drafting);
        }

        void operator()(const cbag::EvalText &v) const {
            oa::oaEvalText::create(block, v.layer, v.purpose, v.text.c_str(),
                                   v.origin, v.alignment, v.orient, v.font, v.height,
                                   v.evaluator.c_str(), v.overbar,
                                   v.visible, v.drafting);
        }

    private:
        oa::oaBlock *block;
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

    void create_terminal_pin(oa::oaBlock *block, const std::map<bsa::name, cbag::PinFigure> &map,
                             int &pin_cnt, const oa::oaCdbaNS &ns) {
        for (auto const &pair : map) {
            // create terminal, net, and pin
            oa::oaName term_name(ns, to_string(pair.first).c_str());
            oa::oaNet *term_net = oa::oaNet::create(block, term_name);
            oa::oaTerm *term = oa::oaTerm::create(term_net, term_name, oa::oacInputTermType);
            oa::oaPin *pin = oa::oaPin::create(term);

            boost::apply_visitor(make_pin_fig_visitor(&ns, block, pin, term, &pin_cnt),
                                 pair.second);
        }
    }

    void OAWriter::write_sch_cellview(const cbag::SchCellView &cv, oa::oaDesign *dsn) {
        oa::oaBlock *block = dsn->getTopBlock();

        int pin_cnt = 0;
        create_terminal_pin(block, cv.in_terms, pin_cnt, ns);
        create_terminal_pin(block, cv.out_terms, pin_cnt, ns);
        create_terminal_pin(block, cv.io_terms, pin_cnt, ns);

        make_shape_visitor shape_visitor(block);
        for (auto const &shape : cv.shapes) {
            boost::apply_visitor(shape_visitor, shape);
        }

        for (auto const &pair : cv.instances) {
            oa::oaScalarName lib(ns, pair.second.lib_name.c_str());
            oa::oaScalarName cell(ns, pair.second.cell_name.c_str());
            oa::oaScalarName view(ns, pair.second.view_name.c_str());
            oa::oaScalarName name(ns, pair.first.base.c_str());
            oa::oaInst *ptr;
            if (pair.first.is_vector()) {
                ptr = oa::oaVectorInst::create(block, lib, cell, view, name,
                                               pair.first.idx_range.start,
                                               pair.first.idx_range.stop, pair.second.xform);
            } else {
                ptr = oa::oaScalarInst::create(block, lib, cell, view, name, pair.second.xform);
            }
            for (auto const &prop_pair : pair.second.params) {
                boost::apply_visitor(make_prop_visitor(ptr, prop_pair.first), prop_pair.second);
            }
        }

        for (auto const &prop_pair : cv.params) {
            boost::apply_visitor(make_prop_visitor(dsn, prop_pair.first), prop_pair.second);
        }
    }
}
