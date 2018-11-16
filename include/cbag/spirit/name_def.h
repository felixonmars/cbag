/** \file name_def.h
 *  \brief This file defines the parsing rule for name.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_NAME_DEF_H
#define CBAG_SPIRIT_NAME_DEF_H

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/ast_adapted.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/name_rep.h>
#include <cbag/spirit/name_unit_def.h>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {
namespace parser {

name_rep_type const name_rep = "name_rep";

name_type const name = "name";

/** Grammar for multiplier tag
 *
 *  The multiplier tag has the form:
 *  <*a>
 *
 *  a must be positive.
 *  expectation operator is used because once the tag prefix is matched,
 *  the context is uniquely determined.
 */
auto const mult_tag = "<*" > x3::uint32[check_zero] > '>';

/** Grammar for a group of names
 *
 *  A group of names is a name_unit or a name surrounded by parentheses.
 *  expectation operator is used to get good error messages.  sequence
 *  operator is used after name_unit to allow for back-tracking, as
 *  we cannot distinguish between name and name_unit until we hit
 *  a comma or the close parenthesis.
 */
auto const grp_name = '(' > ((name_unit >> ')') | (name > ')'));

/** Grammar for a repeated name
 *
 *  name_rep can be one of:
 *  foo
 *  <*a>foo
 *  <*a>(foo)
 *  <*a>(foo,bar)
 *
 *  sequence operator is used in the first OR block to allow for
 *  back-tracking, as we cannot distinguish between a group of names
 *  or a name unit until we see the character right after the multiplier
 *  tag.
 */
auto const name_rep_def = name_rep_type{} = (mult_tag >> grp_name) | (-mult_tag > name_unit);

/** Grammar for name.
 *
 *  a name is simply a comma-separated list of name_reps.
 */
auto const name_def = name_rep % ',';

BOOST_SPIRIT_DEFINE(name_rep);
BOOST_SPIRIT_DEFINE(name);

struct name_rep_class : x3::annotate_on_success, error_handler_base {};
struct name_class : x3::annotate_on_success, error_handler_base {};
} // namespace parser
} // namespace spirit
} // namespace cbag
#endif // CBAG_SPIRIT_NAME_DEF_H
