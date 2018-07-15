/** \file write_oa.h
 *  \brief This file writes CBAG objects to OpenAccess Database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#ifndef CBAGOA_WRITE_OA_H
#define CBAGOA_WRITE_OA_H

#include <spdlog/spdlog.h>

#include <oa/oaDesignDB.h>

#include <cbag/database/figures.h>
#include <cbag/database/cellviews.h>


namespace bsa = cbag::spirit::ast;

namespace cbagoa {

    class OAWriter {
    public:

        OAWriter(oa::oaCdbaNS ns, std::shared_ptr<spdlog::logger> logger)
                : ns(std::move(ns)), logger(std::move(logger)) {};

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

        // Write method for schematic/symbol cell view

        void write_sch_cellview(const cbag::SchCellView &cv, oa::oaDesign *dsn);

    private:
        const oa::oaCdbaNS ns;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif //CBAGOA_WRITE_OA_H
