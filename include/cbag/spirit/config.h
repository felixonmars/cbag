//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_CONFIG_H
#define CBAG_SPIRIT_CONFIG_H

#include <boost/spirit/home/x3.hpp>

namespace cbag {
    namespace spirit {
        namespace parser {
            namespace x3 = boost::spirit::x3;

            using iterator_type = std::string::const_iterator;
            using context_type = x3::unused_type;
        }
    }
}

#endif //CBAG_SPIRIT_CONFIG_H
