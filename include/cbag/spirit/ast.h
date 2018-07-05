//
// Created by erichang on 7/4/18.
//

#ifndef CBAG_SPIRIT_AST_H
#define CBAG_SPIRIT_AST_H

#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace cbag {
    namespace spirit {
        namespace ast {

            namespace x3 = boost::spirit::x3;

            struct range : x3::position_tagged {
                unsigned int start = 0;
                boost::optional<unsigned int> stop;
                boost::optional<unsigned int> step;
            };

            using boost::fusion::operator<<;
        }
    }
}


#endif //CBAG_SPIRIT_AST_H
