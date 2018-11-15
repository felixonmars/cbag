/** \file name_rep.cpp
 *  \brief This file instantiates the parsing rule for name_rep.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <cbag/spirit/config.h>
#include <cbag/spirit/name_rep_def.h>

namespace cbag {
namespace spirit {
namespace parser {
BOOST_SPIRIT_INSTANTIATE(name_rep_type, iterator_type, context_type);
}

parser::name_rep_type const &name_rep() { return parser::name_rep; }
} // namespace spirit
} // namespace cbag
