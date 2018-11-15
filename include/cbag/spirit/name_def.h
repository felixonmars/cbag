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

auto const mult_def = "<*" > (x3::uint32[check_zero]) > '>';

/** Grammar for name_rep
 *
 *  name_rep has the form of <*N>base<a:b:c>.  The multiplier and index range
 * are optional. the multiplier cannot be 0.
 */
auto const name_rep_def = name_rep_type{} =
    ((mult_def >> '(') > ((name_unit >> ')') | (name >> ')'))) | (-mult_def > name_unit);

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
