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

#include <cbag/common/datatypes.h>
#include <cbag/schematic/shape_t.h>

// forward declare structures to reduce dependencies
namespace spdlog {
class logger;
}

namespace cbag {
namespace sch {
class instance;
class pin_figure;
class cellview;
} // namespace sch
} // namespace cbag

namespace cbagoa {

// Read method for properties

std::pair<std::string, cbag::value_t> read_prop(oa::oaProp *p);

std::pair<std::string, cbag::value_t> read_app_def(oa::oaDesign *dsn, oa::oaAppDef *p);

// Read methods for shapes

cbag::sch::rectangle read_rect(oa::oaRect *p, std::string &&net);

cbag::sch::polygon read_poly(oa::oaPolygon *p, std::string &&net);

cbag::sch::arc read_arc(oa::oaArc *p, std::string &&net);

cbag::sch::donut read_donut(oa::oaDonut *p, std::string &&net);

cbag::sch::ellipse read_ellipse(oa::oaEllipse *p, std::string &&net);

cbag::sch::line read_line(oa::oaLine *p, std::string &&net);

cbag::sch::path read_path(oa::oaPath *p, std::string &&net);

cbag::sch::text_t read_text(oa::oaText *p, std::string &&net);

cbag::sch::eval_text read_eval_text(oa::oaEvalText *p, std::string &&net);

cbag::sch::shape_t read_shape(const oa::oaCdbaNS &ns, spdlog::logger &logger, oa::oaShape *p);

// Read method for references

cbag::sch::instance read_instance(const oa::oaCdbaNS &ns, spdlog::logger &logger, oa::oaInst *p);

std::pair<std::string, cbag::sch::instance>
read_instance_pair(const oa::oaCdbaNS &ns, spdlog::logger &logger, oa::oaInst *p);

// Read method for pin figures

cbag::sch::pin_figure read_pin_figure(const oa::oaCdbaNS &ns, spdlog::logger &logger, oa::oaTerm *t,
                                      oa::oaPinFig *p);

// Read method for terminals

std::pair<std::string, cbag::sch::pin_figure>
read_terminal_single(const oa::oaCdbaNS &ns, spdlog::logger &logger, oa::oaTerm *term);

// Read method for schematic/symbol cell view

cbag::sch::cellview read_sch_cellview(const oa::oaCdbaNS &ns, spdlog::logger &logger,
                                      oa::oaDesign *p);

} // namespace cbagoa

#endif // CBAGOA_READ_OA_H
