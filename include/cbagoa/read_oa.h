/** \file read_oa.h
 *  \brief This file converts OpenAccess objects to CBAG data structure.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#ifndef CBAGOA_READ_OA_H
#define CBAGOA_READ_OA_H

#include <memory>

#include <oa/oaDesignDB.h>

#include <cbag/cbag.h>

// forward declare structures to reduce dependencies
namespace spdlog {
class logger;
}

namespace cbagoa {

class OAReader {
  public:
    explicit inline OAReader(oa::oaCdbaNS ns, std::shared_ptr<spdlog::logger> logger)
        : ns(std::move(ns)), logger(std::move(logger)){};

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

    std::pair<std::string, cbag::Instance> read_instance_pair(oa::oaInst *p);

    // Read method for pin figures

    cbag::PinFigure read_pin_figure(oa::oaTerm *t, oa::oaPinFig *p);

    // Read method for terminals

    std::pair<std::string, cbag::PinFigure> read_terminal_single(oa::oaTerm *term);

    // Read method for schematic/symbol cell view

    cbag::SchCellView read_sch_cellview(oa::oaDesign *design);

  private:
    void print_app_def(oa::oaDesign *dsn, oa::oaAppDef *p);

    void print_prop(oa::oaProp *p);

    void print_group(oa::oaGroup *p);

    void print_dm_data(oa::oaDMData *data);

    const oa::oaCdbaNS ns;
    std::shared_ptr<spdlog::logger> logger;
};
} // namespace cbagoa

#endif // CBAGOA_READ_OA_H
