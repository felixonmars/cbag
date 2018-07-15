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

        // Write method for schematic/symbol cell view

        void write_sch_cellview(const cbag::SchCellView &cv, oa::oaDesign *dsn);

    private:
        const oa::oaCdbaNS ns;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif //CBAGOA_WRITE_OA_H
