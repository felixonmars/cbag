/** \file read_oa.h
 *  \brief This file converts OpenAccess objects to CBAG data structure.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#ifndef CBAGOA_READ_OA_H
#define CBAGOA_READ_OA_H

 #include <oa/oaDesignDB.h>

#include <cbag/spirit/ast.h>
#include <cbag/database/figures.h>
#include <cbag/database/cellviews.h>


namespace bsa = cbag::spirit::ast;

namespace cbagoa {

    class OAReader {
    public:

        explicit OAReader(oa::oaCdbaNS ns) : ns(std::move(ns)) {};

        // String parsing methinds

        bsa::name parse_name(const oa::oaString &source);

        bsa::name_unit parse_name_unit(const oa::oaString &source);

        // Read method for properties

        std::pair<std::string, cbag::value_t> read_prop(oa::oaProp *p);

        std::pair<std::string, cbag::value_t> read_app_def(oa::oaDesign *dsn, oa::oaAppDef *p);

        // Read methods for shapes

        cbag::Rect read_rect(oa::oaRect *p, std::string &&net);

        cbag::Poly read_poly(oa::oaPolygon *p, std::string &&net);

        cbag::Arc read_arc(oa::oaArc *p, std::string &&net);

        cbag::Donut read_donut(oa::oaDonut *p, std::string &&net);

        cbag::Ellipse read_ellipse(oa::oaEllipse *p, std::string &&net);

        cbag::Line read_line(oa::oaLine *p, std::string &&net);

        cbag::Path read_path(oa::oaPath *p, std::string &&net);

        cbag::Text read_text(oa::oaText *p, std::string &&net);

        cbag::EvalText read_eval_text(oa::oaEvalText *p, std::string &&net);

        cbag::Shape read_shape(oa::oaShape *p);

        // Read method for references

        cbag::Instance read_instance(oa::oaInst *p);

        std::pair<bsa::name_unit, cbag::Instance> read_instance_pair(oa::oaInst *p);

        // Read method for pin figures

        cbag::PinFigure read_pin_figure(oa::oaTerm *t, oa::oaPinFig *p);

        // Read method for terminals

        std::pair<bsa::name, cbag::PinFigure> read_terminal_single(oa::oaTerm *term);

        // Read method for schematic/symbol cell view

        cbag::SchCellView read_sch_cellview(oa::oaDesign *design);

    private:
        const oa::oaCdbaNS ns;
    };
}

#endif //CBAGOA_READ_OA_H
