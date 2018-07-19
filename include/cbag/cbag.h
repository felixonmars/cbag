/** \file cbag.h
 *  \brief This is the top level header file for cbag library.
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#ifndef CBAG_CBAG_H
#define CBAG_CBAG_H

#include <cbag/spirit/ast.h>
#include <cbag/database/primitives.h>
#include <cbag/database/datatypes.h>
#include <cbag/database/shapes.h>
#include <cbag/database/figures.h>
#include <cbag/database/cellviews.h>


namespace cbag {
    void init_logging();

    void to_file(const SchCellView &cv, const char *fname);

    spirit::ast::name_unit parse_cdba_name_unit(const std::string &source);
}
#endif //CBAG_CBAG_H
