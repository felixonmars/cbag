//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_RANGE_DEF_H
#define CBAG_SPIRIT_RANGE_DEF_H

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/ast_adapted.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/range.h>

namespace cbag {
    namespace spirit {
        namespace parser {
            namespace x3 = boost::spirit::x3;

            range_type const range = "range";

            auto const range_def = '<' > x3::uint_ >> -(':' > x3::uint_ >> -(':' > x3::uint_)) > '>';

            BOOST_SPIRIT_DEFINE(range);

            struct range_class : x3::annotate_on_success, error_handler_base {
            };
        }

        parser::range_type const &range() {
            return parser::range;
        }
    }
}
#endif //CBAG_SPIRIT_RANGE_DEF_H
