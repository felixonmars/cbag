//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_RANGE_DEF_H
#define CBAG_SPIRIT_RANGE_DEF_H

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/ast_adapted.h>
#include <cbag/spirit/range.h>

namespace cbag {
    namespace spirit {
        namespace parser {
            namespace x3 = boost::spirit::x3;
            namespace ascii = boost::spirit::x3::ascii;

            using x3::uint_;

            x3::rule<class range, ast::range> const range = "range";

            auto const range_def = '<' >> uint_ >> -(':' >> uint_ >> -(':' >> uint_)) >> '>';

            BOOST_SPIRIT_DEFINE(range);
        }

        parser::range_type range() {
            return parser::range;
        }
    }
}
#endif //CBAG_SPIRIT_RANGE_DEF_H
