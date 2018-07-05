//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_GRAMMAR_DEF_H
#define CBAG_SPIRIT_GRAMMAR_DEF_H

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/ast_adapted.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/range.h>
#include <cbag/spirit/grammar.h>

namespace cbag {
    namespace spirit {
        namespace parser {
            namespace x3 = boost::spirit::x3;

            range_type const range = "range";

            name_unit_type const name_unit = "name_unit";

            auto const name_string = +(x3::ascii::print - x3::ascii::char_("<>:*"));

            auto const range_def = '<' > x3::uint_ >> -(':' > x3::uint_ >> -(':' > x3::uint_)) > '>';

            auto const name_unit_def = name_string >> -(range);

            BOOST_SPIRIT_DEFINE(range, name_unit);

            struct range_class : x3::annotate_on_success, error_handler_base {
            };

            struct name_unit_class : x3::annotate_on_success, error_handler_base {
            };
        }

        parser::range_type const &range() {
            return parser::range;
        }

        parser::name_unit_type const &name_unit() {
            return parser::name_unit;
        }
    }
}
#endif //CBAG_SPIRIT_GRAMMAR_DEF_H
