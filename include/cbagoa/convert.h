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


namespace bsa = cbag::spirit::ast;

namespace cbagoa {

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

    cbag::Shape read_shape(oa::oaShape *shape_ptr);

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

    // Read method for terminals

    std::pair<bsa::name, std::vector<cbag::PinFigure>> read_terminals(oa::oaBlock *block);

    // Read method for properties

    std::pair<std::string, cbag::value_t> read_prop(oa::oaProp *prop_ptr);
}

#endif //CBAGOA_CONVERT_H
