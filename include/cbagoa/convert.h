/** \file convert.h
 *  \brief This file defines conversion methods between OpenAccess and internal data structure.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#ifndef CBAGOA_CONVERT_H
#define CBAGOA_CONVERT_H

#include <oa/oaDesignDB.h>

#include <cbag/spirit/ast.h>
#include <cbag/database/figures.h>
#include <cbag/database/cellviews.h>


namespace bsa = cbag::spirit::ast;

namespace cbagoa {

    // String parsing methinds

    bsa::name parse_name(const oa::oaString &source);

    bsa::name_unit parse_name_unit(const oa::oaString &source);

    // Read method for properties

    std::pair<std::string, cbag::value_t> read_prop(oa::oaProp *prop_ptr);

    // Read methods for shapes

    cbag::Rect read_rect(oa::oaRect *p);

    cbag::Poly read_poly(oa::oaPolygon *p);

    cbag::Arc read_arc(oa::oaArc *p);

    cbag::Donut read_donut(oa::oaDonut *p);

    cbag::Ellipse read_ellipse(oa::oaEllipse *p);

    cbag::Line read_line(oa::oaLine *p);

    cbag::Path read_path(oa::oaPath *p);

    cbag::Text read_text(oa::oaText *p);

    cbag::EvalText read_eval_text(oa::oaEvalText *p);

    cbag::Shape read_shape(oa::oaShape *p, const oa::oaNameSpace &ns);

    // Read method for references

    cbag::Instance read_instance(oa::oaInst *p, const oa::oaNameSpace &ns);

    std::pair<bsa::name_unit, cbag::Instance> read_instance_pair(oa::oaInst *p,
                                                                 const oa::oaNameSpace &ns);
    // Read method for pin figures

    cbag::PinFigure read_pin_figure(oa::oaPinFig *p, const oa::oaNameSpace &ns);

    // Read method for terminals

    std::pair<bsa::name, cbag::PinFigure> read_terminal_single(oa::oaTerm *term,
                                                               const oa::oaNameSpace &ns);

    // Read method for schematic/symbol cell view

    cbag::SchCellView read_sch_cell_view(oa::oaDesign *design, const oa::oaNameSpace &ns);

    // Write methods for shapes

    oa::oaRect *write_rect(oa::oaBlock *block, const cbag::Rect &v);

    oa::oaPolygon *write_poly(oa::oaBlock *block, const cbag::Poly &v);

    oa::oaArc *write_arc(oa::oaBlock *block, const cbag::Arc &v);

    oa::oaDonut *write_donut(oa::oaBlock *block, const cbag::Donut &v);

    oa::oaEllipse *write_ellipse(oa::oaBlock *block, const cbag::Ellipse &v);

    oa::oaLine *write_line(oa::oaBlock *block, const cbag::Line &v);

    oa::oaPath *write_path(oa::oaBlock *block, const cbag::Path &v);

    oa::oaText *write_text(oa::oaBlock *block, const cbag::Text &v);

    oa::oaEvalText *write_eval_text(oa::oaBlock *block, const cbag::EvalText &v);
}

#endif //CBAGOA_CONVERT_H
