/** \file name.cpp
 *  \brief This file instantiates the parsing rule for name.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <cbag/spirit/config.h>
#include <cbag/spirit/name_def.h>

namespace cbag {
namespace spirit {
namespace parser {
BOOST_SPIRIT_INSTANTIATE(name_rep_type, iterator_type, context_type);

BOOST_SPIRIT_INSTANTIATE(name_type, iterator_type, context_type);
} // namespace parser

parser::name_rep_type const &name_rep() { return parser::name_rep; }

parser::name_type const &name() { return parser::name; }
} // namespace spirit
} // namespace cbag
