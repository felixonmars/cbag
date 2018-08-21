/** \file write_oa.h
 *  \brief This file writes CBAG objects to OpenAccess Database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#ifndef CBAGOA_WRITE_OA_H
#define CBAGOA_WRITE_OA_H

#include <oa/oaDesignDB.h>

#include <cbag/cbag.h>

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

class OAWriter {
  public:
    explicit OAWriter(oa::oaCdbaNS ns) : ns(std::move(ns)){};

    // Write method for schematic/symbol cell view

    void write_sch_cellview(const cbag::SchCellView &cv, oa::oaDesign *dsn, bool is_sch);

  private:
    void create_terminal_pin(oa::oaBlock *block, int &pin_cnt,
                             const std::map<std::string, cbag::PinFigure> &map,
                             oa::oaTermTypeEnum term_type);

    const oa::oaCdbaNS ns;
};
} // namespace cbagoa

#endif // CBAGOA_WRITE_OA_H
