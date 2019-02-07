/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <cbag/spirit/ast.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/name_rep.h>
#include <cbag/spirit/name_unit.h>
#include <cbag/spirit/parsers.h>
#include <cbag/util/name_convert.h>

namespace cbag {
namespace util {

spirit::ast::name_unit parse_cdba_name_unit(const std::string &source) {
    spirit::ast::name_unit ast;
    parse(source, spirit::name_unit(), ast);
    return ast;
}

spirit::ast::name parse_cdba_name(const std::string &source) {
    spirit::ast::name ast;
    parse(source, spirit::name(), ast);
    return ast;
}

spirit::ast::name_rep parse_cdba_name_rep(const std::string &source) {
    spirit::ast::name_rep ast;
    parse(source, spirit::name_rep(), ast);
    return ast;
}

} // namespace util
} // namespace cbag
