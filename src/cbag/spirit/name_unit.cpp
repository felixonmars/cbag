/** \file name_unit.cpp
 *  \brief This file instantiates the parsing rule for name_unit.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <cbag/spirit/config.h>
#include <cbag/spirit/name_unit_def.h>


namespace cbag {
    namespace spirit {
        namespace parser {
            BOOST_SPIRIT_INSTANTIATE(name_unit_type, iterator_type, context_type);
        }

        parser::name_unit_type const &name_unit() {
            return parser::name_unit;
        }
    }
}
