//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_CONFIG_H
#define CBAG_SPIRIT_CONFIG_H

#include <cbag/spirit/error_handler.h>


namespace x3 = boost::spirit::x3;

namespace cbag {
    namespace spirit {
        namespace parser {

            using iterator_type = std::string::const_iterator;
            using error_handler_type = error_handler<iterator_type>;
            using context_type = x3::context<error_handler_tag,
                    std::reference_wrapper<error_handler_type> const,
                    x3::unused_type>;
        }
    }
}

#endif //CBAG_SPIRIT_CONFIG_H
