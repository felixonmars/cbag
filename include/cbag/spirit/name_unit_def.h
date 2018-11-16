#ifndef CBAG_SPIRIT_NAME_UNIT_DEF_H
#define CBAG_SPIRIT_NAME_UNIT_DEF_H

#include <cctype>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/ast_adapted.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/name_unit.h>
#include <cbag/spirit/range_def.h>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {
namespace parser {

name_unit_type const name_unit = "name_unit";

auto check_str = [](auto &ctx) { x3::_pass(ctx) = (std::isalpha(x3::_attr(ctx).at(0)) != 0); };

/** A string with no spaces, parentheses, angle brackets, colon, commas, or stars.
 */
auto const name_string = +(x3::ascii::print - x3::ascii::char_("()<>:*, "));

/** Grammar for name_unit
 *
 *  name_unit can be one of:
 *
 *  foo
 *  foo<__range__>
 *
 *  the first charater of the base name must be an alphabet.
 */
auto const name_unit_def = name_unit_type{} = name_string[check_str] >> -(range);

BOOST_SPIRIT_DEFINE(name_unit);

struct name_unit_class : x3::annotate_on_success, error_handler_base {};

} // namespace parser
} // namespace spirit
} // namespace cbag
#endif
