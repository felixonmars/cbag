//
// Created by erichang on 7/5/18.
//

#include <cbag/spirit/config.h>
#include <cbag/spirit/grammar_def.h>

namespace cbag {
    namespace spirit {
        namespace parser {
            BOOST_SPIRIT_INSTANTIATE(range_type, iterator_type, context_type);

            BOOST_SPIRIT_INSTANTIATE(name_unit_type, iterator_type, context_type);
        }
    }
}
