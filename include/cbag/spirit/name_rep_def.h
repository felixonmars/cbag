/** \file name_rep_def.h
 *  \brief This file defines the parsing rule for name_rep.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_NAME_REP_DEF_H
#define CBAG_SPIRIT_NAME_REP_DEF_H

#include <cctype>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/ast_adapted.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/name_rep.h>
#include <cbag/spirit/range_def.h>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {
namespace parser {

name_rep_type const name_rep = "name_rep";

auto check_str = [](auto &ctx) { x3::_pass(ctx) = (std::isalpha(x3::_attr(ctx).at(0)) != 0); };

/** A string with no spaces, '<', '>', ':', ',', or '*'.
 */
auto const name_string = +(x3::ascii::print - x3::ascii::char_("<>:*, "));

/** Grammar for name_rep
 *
 *  name_rep has the form of <*N>base<a:b:c>.  The multiplier and index range
 * are optional. the multiplier cannot be 0.
 */
auto const name_rep_def = name_rep_type{} = -("<*" > (x3::uint32[check_zero]) > ">") >
                                            name_string[check_str] >> -(range);

BOOST_SPIRIT_DEFINE(name_rep);

struct name_rep_class : x3::annotate_on_success, error_handler_base {};
} // namespace parser
} // namespace spirit
} // namespace cbag
#endif // CBAG_SPIRIT_NAME_REP_DEF_H
