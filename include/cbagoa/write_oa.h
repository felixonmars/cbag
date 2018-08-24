/** \file write_oa.h
 *  \brief This file writes CBAG objects to OpenAccess Database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#ifndef CBAGOA_WRITE_OA_H
#define CBAGOA_WRITE_OA_H

#include <memory>
#include <unordered_map>

#include <oa/oaDesignDB.h>

#include <cbag/cbag.h>

// forward declare structures to reduce dependencies
namespace spdlog {
class logger;
} // namespace spdlog

namespace cbagoa {
// TODO: find ways to not hard code these values
constexpr oa::oaLayerNum sch_conn_layer = 228;
constexpr oa::oaPurposeNum sch_conn_purpose = 4294967295;
constexpr oa::oaPurposeNum sch_net_purpose = 237;
constexpr oa::oaCoord sch_stub_len2 = 5;
constexpr oa::oaTextAlignEnum sch_net_align = oa::oacCenterCenterTextAlign;
constexpr oa::oaOrientEnum sch_net_orient = oa::oacR0;
constexpr oa::oaFontEnum sch_net_font = oa::oacStickFont;
constexpr oa::oaDist sch_net_height = 10;

constexpr auto prop_app_type = "ILList";
constexpr auto cell_data_name = "cdfData";
constexpr auto sch_data_parent_name = "dependency";
constexpr auto sch_data_name = "children";
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

using str_map_t = std::unordered_map<std::string, std::string>;

class OAWriter {
  public:
    explicit inline OAWriter(oa::oaCdbaNS ns, std::shared_ptr<spdlog::logger> logger)
        : ns(std::move(ns)), logger(std::move(logger)){};

    // Write method for schematic/symbol cell view

    void write_sch_cellview(const cbag::SchCellView &cv, oa::oaDesign *dsn, bool is_sch,
                            const str_map_t *rename_map = nullptr);

  private:
    void create_terminal_pin(oa::oaBlock *block, int &pin_cnt,
                             const std::map<std::string, cbag::PinFigure> &map,
                             oa::oaTermTypeEnum term_type);

    void write_sch_cell_data(const cbag::SchCellView &cv, const oa::oaScalarName &lib_name,
                             const oa::oaScalarName &cell_name, const oa::oaScalarName &view_name,
                             const std::string &term_order);

    const oa::oaCdbaNS ns;
    std::shared_ptr<spdlog::logger> logger;
};
} // namespace cbagoa

#endif // CBAGOA_WRITE_OA_H
