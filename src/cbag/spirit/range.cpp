/** \file range.cpp
 *  \brief This file instantiates the parsing rule for range.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#include <cbag/spirit/config.h>
#include <cbag/spirit/range_def.h>

namespace cbag {
namespace spirit {
namespace parser {
BOOST_SPIRIT_INSTANTIATE(range_type, iterator_type, context_type);
}

parser::range_type const &range() { return parser::range; }
} // namespace spirit
} // namespace cbag
