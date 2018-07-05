//
// Created by erichang on 7/5/18.
//

#include <cbag/spirit/range_def.h>
#include <cbag/spirit/config.h>

namespace cbag {
    namespace spirit {
        namespace parser {
            BOOST_SPIRIT_INSTANTIATE(range_type, iterator_type, context_type);
        }
    }
}
