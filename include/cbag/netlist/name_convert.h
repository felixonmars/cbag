/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_NAME_CONVERT_H
#define CBAG_NETLIST_NAME_CONVERT_H

#include <string>

#include <cbag/spirit/ast.h>

namespace cbag {

// string <-> name conversion methods

spirit::ast::name_unit parse_cdba_name_unit(const std::string &source);

spirit::ast::name parse_cdba_name(const std::string &source);

spirit::ast::name parse_cdba_name(const char *source);

std::string to_string_cdba(const spirit::ast::name_bit &bit);
} // namespace cbag

#endif // CBAG_NETLIST_NAME_CONVERT_H
