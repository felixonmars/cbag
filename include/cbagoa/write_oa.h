/** \file write_oa.h
 *  \brief This file writes CBAG objects to OpenAccess Database.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#ifndef CBAGOA_WRITE_OA_H
#define CBAGOA_WRITE_OA_H

#include <oa/oaDesignDB.h>

#include <easylogging++.h>

#include <cbag/database/figures.h>
#include <cbag/database/cellviews.h>


namespace bsa = cbag::spirit::ast;

namespace cbagoa {

    class OAWriter {
    public:

        OAWriter(oa::oaCdbaNS ns, el::Logger *logger)
                : ns(std::move(ns)), logger(logger) {};

        // Write method for schematic/symbol cell view

        void write_sch_cellview(const cbag::SchCellView &cv, oa::oaDesign *dsn);

    private:
        const oa::oaCdbaNS ns;
        el::Logger *logger;
    };
}

#endif //CBAGOA_WRITE_OA_H
