/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <fmt/format.h>

#include <cbag/netlist/name_convert.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/name_unit.h>
#include <cbag/spirit/parsers.h>

namespace cbag {

spirit::ast::name_unit parse_cdba_name_unit(const std::string &source) {
    spirit::ast::name_unit ast;
    parse(source.c_str(), source.size(), spirit::name_unit(), ast);
    return ast;
}

spirit::ast::name_unit parse_cdba_name_unit(const char *source) {
    spirit::ast::name_unit ast;
    parse(source, strlen(source), spirit::name_unit(), ast);
    return ast;
}

spirit::ast::name parse_cdba_name(const std::string &source) {
    spirit::ast::name ast;
    parse(source.c_str(), source.size(), spirit::name(), ast);
    return ast;
}

spirit::ast::name parse_cdba_name(const char *source) {
    spirit::ast::name ast;
    parse(source, strlen(source), spirit::name(), ast);
    return ast;
}

std::string to_string_cdba(const spirit::ast::name_bit &bit) {
    if (bit.index) {
        return fmt::format("{}<{}>", bit.base, *bit.index);
    }
    return std::string(bit.base);
}

} // namespace cbag
