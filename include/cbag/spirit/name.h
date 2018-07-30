/** \file name.h
 *  \brief This file declares the parsing rule for name.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_NAME_H
#define CBAG_SPIRIT_NAME_H

#include <boost/spirit/home/x3.hpp>

#include <cbag/spirit/ast.h>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {
namespace parser {

struct name_class;

using name_type = x3::rule<name_class, ast::name, true>;

BOOST_SPIRIT_DECLARE(name_type);
} // namespace parser

parser::name_type const &name();
} // namespace spirit
} // namespace cbag

#endif // CBAG_SPIRIT_NAME_H
