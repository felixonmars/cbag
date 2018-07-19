/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_DATABASE_CELLVIEWS_H
#define CBAG_DATABASE_CELLVIEWS_H

#include <map>

#include <cbag/database/datatypes.h>
#include <cbag/database/shapes.h>


namespace cbag {

    struct PinFigure;
    struct Instance;

    /** A schematic or symbol cell view
     *
     */
    struct SchCellView {
        SchCellView() = default;

        explicit SchCellView(const char *yaml_fname);

        std::map<std::string, PinFigure> in_terms, out_terms, io_terms;
        std::vector<Shape> shapes;
        std::map<std::string, Instance> instances;
        ParamMap props;
        ParamMap app_defs;
    };
}

#endif //CBAG_DATABASE_CELLVIEWS_H
