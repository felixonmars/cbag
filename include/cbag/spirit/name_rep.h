/** \file name_rep.h
 *  \brief This file declares the parsing rule for name_rep.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_NAME_REP_H
#define CBAG_SPIRIT_NAME_REP_H

#include <boost/spirit/home/x3.hpp>

#include <cbag/spirit/ast.h>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {
namespace parser {

struct name_rep_class;

using name_rep_type = x3::rule<name_rep_class, ast::name_rep, true>;

BOOST_SPIRIT_DECLARE(name_rep_type);
} // namespace parser

parser::name_rep_type const &name_rep();
} // namespace spirit
} // namespace cbag

#endif // CBAG_SPIRIT_NAME_REP_H
