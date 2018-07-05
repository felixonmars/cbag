//
// Created by erichang on 7/4/18.
//

#ifndef CBAG_SPIRIT_AST_H
#define CBAG_SPIRIT_AST_H

#include <boost/fusion/include/io.hpp>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include <iostream>
#include <string>

namespace cbag {
    namespace spirit {
        namespace ast {

            struct range {
                uint32_t start = 0;
                boost::optional<uint32_t> stop;
                boost::optional<uint32_t> step;
            };

            using boost::fusion::operator<<;
        }
    }
}


#endif //CBAG_SPIRIT_AST_H
