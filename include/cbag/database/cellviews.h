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

    /** A schematic or symbol cell view
     *
     */
    struct SchCellView {
        SchCellView() = default;

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(in_terms);
            ar & BOOST_SERIALIZATION_NVP(out_terms);
            ar & BOOST_SERIALIZATION_NVP(io_terms);
            ar & BOOST_SERIALIZATION_NVP(shapes);
            ar & BOOST_SERIALIZATION_NVP(instances);
            ar & BOOST_SERIALIZATION_NVP(params);
#pragma clang diagnostic pop
        }

        std::map<bsa::name, PinFigure> in_terms, out_terms, io_terms;
        std::vector<Shape> shapes;
        std::map<bsa::name_unit, Instance> instances;
        ParamMap params;
    };
}

#endif //CBAG_DATABASE_CELLVIEWS_H
