/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_DATABASE_CELLVIEWS_H
#define CBAG_DATABASE_CELLVIEWS_H

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>


#include <cbag/spirit/ast.h>
#include <cbag/database/datatypes.h>
#include <cbag/database/shapes.h>


namespace bsa = cbag::spirit::ast;

namespace cbag {

    /** An instance in a schematic.
     */
    struct SchInstance {
        /** Create an empty instance.
         */
        SchInstance() = default;

        /** Create an instance with empty parameter and terminal mappings.
         *
         * @param lib the library name.
         * @param cell the cell name.
         * @param view the view name.
         * @param xform the instance location.
         */
        SchInstance(std::string &&lib, std::string &&cell, std::string &&view, Transform xform)
                : lib_name(lib), cell_name(cell), view_name(view), xform(xform), connections({}),
                  params({}) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(lib_name);
            ar & BOOST_SERIALIZATION_NVP(cell_name);
            ar & BOOST_SERIALIZATION_NVP(view_name);
            ar & BOOST_SERIALIZATION_NVP(xform);
            ar & BOOST_SERIALIZATION_NVP(connections);
            ar & BOOST_SERIALIZATION_NVP(params);
#pragma clang diagnostic pop
        }

        std::string lib_name, cell_name, view_name;
        Transform xform;
        std::map<bsa::name_bit, std::vector<bsa::name_bit>> connections;
        ParamMap params;
    };

    /** A schematic master, in other words, a schematic cellview.
     *
     *  Note that the terminal lists and instance list are all sorted in ascending order.
     */
    struct SchMaster {
        SchMaster() = default;

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(symbols);
            ar & BOOST_SERIALIZATION_NVP(in_pins);
            ar & BOOST_SERIALIZATION_NVP(out_pins);
            ar & BOOST_SERIALIZATION_NVP(io_pins);
            ar & BOOST_SERIALIZATION_NVP(inst_map);
#pragma clang diagnostic pop
        }

        std::set<std::string> symbols;
        std::set<bsa::name> in_pins, out_pins, io_pins;
        std::map<bsa::name_unit, SchInstance> inst_map;
    };

    /** A schematic symbol, in other words, a symbol cellview.
     *
     */
    struct SchSymbol {
        SchSymbol() = default;

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(in_pins);
            ar & BOOST_SERIALIZATION_NVP(out_pins);
            ar & BOOST_SERIALIZATION_NVP(io_pins);
#pragma clang diagnostic pop
        }

        std::map<bsa::name, RectShape> in_pins, out_pins, io_pins;
    };

}

#endif //CBAG_DATABASE_CELLVIEWS_H
