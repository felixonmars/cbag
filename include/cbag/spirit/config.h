//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_CONFIG_H
#define CBAG_SPIRIT_CONFIG_H

#include <boost/spirit/home/x3.hpp>

#include <cbag/spirit/error_handler.h>

namespace cbag {
    namespace spirit {
        namespace parser {
            namespace x3 = boost::spirit::x3;

            using iterator_type = std::string::const_iterator;
            using phrase_context_type = x3::phrase_parse_context<x3::ascii::space_type>::type;
            using error_handler_type = error_handler<iterator_type>;
            typedef x3::context<error_handler_tag, std::reference_wrapper<error_handler_type>,
                    phrase_context_type> context_type;
        }
    }
}

#endif //CBAG_SPIRIT_CONFIG_H
