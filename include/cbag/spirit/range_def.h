/** \file range_def.h
 *  \brief This file defines the parsing rule for range.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_RANGE_DEF_H
#define CBAG_SPIRIT_RANGE_DEF_H

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/ast_adapted.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/range.h>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {
namespace parser {

// after parsing start, set default values of stop and step
auto init_range = [](auto &ctx) {
    x3::_val(ctx).stop = x3::_attr(ctx);
    x3::_val(ctx).step = 1;
};

// make sure value is not 0
auto check_zero = [](auto &ctx) { x3::_pass(ctx) = (x3::_attr(ctx) != 0); };

range_type const range = "range";

/** Grammer for range
 *
 *  range is of the form <a:b:c>, b and c are optional.
 *  if b is missing, then b = a by default.  If c is missing, then c = 1.
 *  c cannot be 0.
 */
auto const range_def = range_type{} = '<' > (x3::uint32[init_range]) >>
                                      -(':' > x3::uint32 >>
                                        -(':' > (x3::uint32[check_zero]))) >
                                      '>';

BOOST_SPIRIT_DEFINE(range);

struct range_class : x3::annotate_on_success, error_handler_base {};
} // namespace parser
} // namespace spirit
} // namespace cbag
#endif // CBAG_SPIRIT_RANGE_DEF_H
