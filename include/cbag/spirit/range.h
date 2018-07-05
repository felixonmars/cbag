//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_RANGE_H
#define CBAG_SPIRIT_RANGE_H

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>

#include <cbag/spirit/ast.h>

namespace cbag {
    namespace spirit {
        namespace parser {
            namespace x3 = boost::spirit::x3;
            using range_type = x3::rule<class range, ast::range>;

            BOOST_SPIRIT_DECLARE(range_type);
        }

        parser::range_type range();
    }
}


#endif //CBAG_SPIRIT_RANGE_H
