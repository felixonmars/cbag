/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_DATABASE_CELLVIEWS_H
#define CBAG_DATABASE_CELLVIEWS_H

#include <map>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/database/datatypes.h>
#include <cbag/database/figures.h>


namespace bsa = cbag::spirit::ast;

namespace cbag {

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
        std::map<bsa::name_unit, Instance> inst_map;
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
            ar & BOOST_SERIALIZATION_NVP(shapes);
            ar & BOOST_SERIALIZATION_NVP(params);
#pragma clang diagnostic pop
        }

        std::map<bsa::name, Rect> in_pins, out_pins, io_pins;
        std::vector<Shape> shapes;
        ParamMap params;
    };

}

#endif //CBAG_DATABASE_CELLVIEWS_H
