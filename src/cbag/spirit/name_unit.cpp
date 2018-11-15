#include <cbag/spirit/config.h>
#include <cbag/spirit/name_unit_def.h>

namespace cbag {
namespace spirit {
namespace parser {

BOOST_SPIRIT_INSTANTIATE(name_unit_type, iterator_type, context_type);
}

parser::name_unit_type const &name_unit() { return parser::name_unit; }

} // namespace spirit
} // namespace cbag
