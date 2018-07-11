/** \file figures.h
 *  \brief This file defines various figure classes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_DATABASE_FIGURES_H
#define CBAG_DATABASE_FIGURES_H

#include <cbag/database/datatypes.h>
#include <cbag/spirit/ast.h>


namespace bsa = cbag::spirit::ast;

namespace cbag {

    /** An instance in a schematic.
     */
    struct CSchInstance {
        /** Create an empty instance.
         */
        CSchInstance() = default;

        /** Create an instance with empty parameter and terminal mappings.
         *
         * @param lib the library name.
         * @param cell the cell name.
         * @param view the view name.
         * @param xform the instance location.
         */
        CSchInstance(std::string &&lib, std::string &&cell, std::string &&view, Transform xform)
                : lib_name(lib), cell_name(cell), view_name(view), xform(xform), connections({}), params({}) {}

        std::string lib_name, cell_name, view_name;
        Transform xform;
        std::map<bsa::name_bit, std::vector<bsa::name_bit>> connections;
        ParamMap params;
    };

    /** A schematic master, in other words, a schematic cellview.
     *
     *  Note that the terminal lists and instance list are all sorted in ascending order.
     */
    struct CSchMaster {
        CSchMaster() = default;

        std::set<std::string> symbols;
        std::string lib_name, cell_name, view_name;
        std::set<bsa::name> in_pins, out_pins, io_pins;
        std::map<bsa::name_unit, CSchInstance> inst_map;
    };

}

#endif //CBAG_DATABASE_FIGURES_H
