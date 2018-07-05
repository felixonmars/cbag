//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_GRAMMAR_H
#define CBAG_SPIRIT_GRAMMAR_H

#include <boost/spirit/home/x3.hpp>

#include <cbag/spirit/ast.h>

namespace cbag {
    namespace spirit {
        namespace parser {
            namespace x3 = boost::spirit::x3;

            struct range_class;

            struct name_unit_class;

            using range_type = x3::rule<range_class, ast::range>;

            using name_unit_type = x3::rule<name_unit_class, ast::name_unit>;

            BOOST_SPIRIT_DECLARE(range_type, name_unit_type);
        }

        parser::range_type const &range();

        parser::name_unit_type const &name_unit();
    }
}


#endif //CBAG_SPIRIT_GRAMMAR_H
