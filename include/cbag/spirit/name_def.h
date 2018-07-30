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
#include <cbag/spirit/name_unit_def.h>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {
namespace parser {

name_type const name = "name";

/** Grammar for name.
 *
 *  a name is simply a comma-separated list of name_units.
 */
auto const name_def = name_unit % ',';

BOOST_SPIRIT_DEFINE(name);

struct name_class : x3::annotate_on_success, error_handler_base {};
} // namespace parser
} // namespace spirit
} // namespace cbag
#endif // CBAG_SPIRIT_NAME_DEF_H
