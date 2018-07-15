/** \file write_oa.cpp
 *  \brief This file writes CBAG data structure to OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#include <utility>

#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/ostream.h>

#include <cbag/spirit/ast.h>

#include <cbagoa/write_oa.h>


namespace bsp = cbag::spirit;
namespace bsa = cbag::spirit::ast;

namespace cbagoa {

    // Write methods for shapes

    oa::oaRect *OAWriter::write_rect(oa::oaBlock *block, const cbag::Rect &v) {
        return oa::oaRect::create(block, v.layer, v.purpose, v.bbox);
    }

    oa::oaPolygon *OAWriter::write_poly(oa::oaBlock *block, const cbag::Poly &v) {
        return oa::oaPolygon::create(block, v.layer, v.purpose, v.points);
    }

    oa::oaArc *OAWriter::write_arc(oa::oaBlock *block, const cbag::Arc &v) {
        return oa::oaArc::create(block, v.layer, v.purpose, v.bbox, v.ang_start, v.ang_stop);
    }

    oa::oaDonut *OAWriter::write_donut(oa::oaBlock *block, const cbag::Donut &v) {
        return oa::oaDonut::create(block, v.layer, v.purpose, v.center, v.radius, v.hole_radius);
    }

    oa::oaEllipse *OAWriter::write_ellipse(oa::oaBlock *block, const cbag::Ellipse &v) {
        return oa::oaEllipse::create(block, v.layer, v.purpose, v.bbox);
    }

    oa::oaLine *OAWriter::write_line(oa::oaBlock *block, const cbag::Line &v) {
        return oa::oaLine::create(block, v.layer, v.purpose, v.points);
    }

    oa::oaPath *OAWriter::write_path(oa::oaBlock *block, const cbag::Path &v) {
        return oa::oaPath::create(block, v.layer, v.purpose, v.width, v.points, v.style,
                                  v.begin_ext, v.end_ext);
    }

    oa::oaText *OAWriter::write_text(oa::oaBlock *block, const cbag::Text &v) {
        return oa::oaText::create(block, v.layer, v.purpose, oa::oaString(v.text.c_str()),
                                  v.origin, v.alignment, v.orient, v.font, v.height, v.overbar,
                                  v.visible, v.drafting);
    }

    oa::oaEvalText *OAWriter::write_eval_text(oa::oaBlock *block, const cbag::EvalText &v) {
        return oa::oaEvalText::create(block, v.layer, v.purpose, oa::oaString(v.text.c_str()),
                                      v.origin, v.alignment, v.orient, v.font, v.height,
                                      oa::oaString(v.evaluator.c_str()), v.overbar,
                                      v.visible, v.drafting);
    }

    void OAWriter::write_sch_cellview(const cbag::SchCellView &cv, oa::oaDesign *dsn) {
        oa::oaBlock *block = dsn->getTopBlock();

        logger->info("Writing input terminals");
        for (const auto pair : cv.in_terms) {
        }

    }
}
