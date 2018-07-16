/** \file write_oa.h
 *  \brief This file writes CBAG objects to OpenAccess Database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#ifndef CBAGOA_WRITE_OA_H
#define CBAGOA_WRITE_OA_H

#include <oa/oaDesignDB.h>

#include <cbag/database/figures.h>
#include <cbag/database/cellviews.h>


namespace bsa = cbag::spirit::ast;

namespace cbagoa {

    class OAWriter {
    public:

        explicit OAWriter(oa::oaCdbaNS ns) : ns(std::move(ns)) {};

        // Write method for schematic/symbol cell view

        void write_sch_cellview(const cbag::SchCellView &cv, oa::oaDesign *dsn);

    private:

        void
        create_terminal_pin(oa::oaBlock *block, int &pin_cnt,
                            const std::map<bsa::name, cbag::PinFigure> &map);

        const oa::oaCdbaNS ns;
    };
}

#endif //CBAGOA_WRITE_OA_H
