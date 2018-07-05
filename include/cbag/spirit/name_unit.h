//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_NAME_UNIT_H
#define CBAG_SPIRIT_NAME_UNIT_H

#include <boost/spirit/home/x3.hpp>

#include <cbag/spirit/ast.h>

namespace cbag {
    namespace spirit {
        namespace parser {
            namespace x3 = boost::spirit::x3;

            struct name_unit_class;

            using name_unit_type = x3::rule<name_unit_class, ast::name_unit>;

            BOOST_SPIRIT_DECLARE(name_unit_type);
        }

        parser::name_unit_type const &name_unit();
    }
}


#endif //CBAG_SPIRIT_NAME_UNIT_H
