//
// Created by erichang on 7/5/18.
//

#include <cbag/spirit/config.h>
#include <cbag/spirit/name_def.h>


namespace cbag {
    namespace spirit {
        namespace parser {
            BOOST_SPIRIT_INSTANTIATE(name_type, iterator_type, context_type);
        }

        parser::name_type const &name() {
            return parser::name;
        }
    }
}
