/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_DATABASE_CELLVIEWS_H
#define CBAG_DATABASE_CELLVIEWS_H

#include <map>

#include <cbag/spirit/ast.h>
#include <cbag/database/datatypes.h>
#include <cbag/database/figures.h>


namespace bsa = cbag::spirit::ast;

namespace cbag {

    /** A schematic or symbol cell view
     *
     */
    struct SchCellView {
        SchCellView() = default;

        std::map<bsa::name, PinFigure> in_terms, out_terms, io_terms;
        std::vector<Shape> shapes;
        std::map<bsa::name_unit, Instance> instances;
        ParamMap props;
        ParamMap app_defs;
    };
}

#endif //CBAG_DATABASE_CELLVIEWS_H
