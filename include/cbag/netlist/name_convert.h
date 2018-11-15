/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_NAME_CONVERT_H
#define CBAG_NETLIST_NAME_CONVERT_H

#include <string>

namespace cbag {

namespace spirit {
namespace ast {
struct name_rep;
struct name;
} // namespace ast
} // namespace spirit

// string <-> name conversion methods

spirit::ast::name_rep parse_cdba_name_rep(const std::string &source);

spirit::ast::name parse_cdba_name(const std::string &source);

std::string to_string_cdba(const spirit::ast::name_rep &nu);

std::string to_string_cdba(const spirit::ast::name &name);

} // namespace cbag

#endif // CBAG_NETLIST_NAME_CONVERT_H
