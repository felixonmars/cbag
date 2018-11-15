/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <fmt/core.h>

#include <cbag/netlist/name_convert.h>
#include <cbag/spirit/ast.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/name_rep.h>
#include <cbag/spirit/parsers.h>

namespace cbag {

spirit::ast::name_rep parse_cdba_name_rep(const std::string &source) {
    spirit::ast::name_rep ast;
    parse(source.c_str(), source.size(), spirit::name_rep(), ast);
    return ast;
}

spirit::ast::name_rep parse_cdba_name_rep(const char *source) {
    spirit::ast::name_rep ast;
    parse(source, strlen(source), spirit::name_rep(), ast);
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

std::string to_string_cdba(const spirit::ast::range &r) {
    if (r.step == 0) {
        return "";
    } else if (r.start == r.stop) {
        return fmt::format("<{}>", r.start);
    } else if (r.step == 1 || r.step == 1) {
        return fmt::format("<{}:{}>", r.start, r.stop);
    } else {
        return fmt::format("<{}:{}:{}>", r.start, r.stop, r.step);
    }
}

std::string to_string_cdba(const spirit::ast::name_bit &bit) {
    if (bit.index) {
        return fmt::format("{}<{}>", bit.base, *bit.index);
    }
    return std::string(bit.base);
}

std::string to_string_cdba(const spirit::ast::name_rep &nu) {
    if (nu.mult == 1) {
        return fmt::format("{}{}", nu.base, to_string_cdba(nu.idx_range));
    } else {
        return fmt::format("<*{}>{}{}", nu.mult, nu.base, to_string_cdba(nu.idx_range));
    }
}

std::string to_string_cdba(const spirit::ast::name &name) {
    std::ostringstream builder;
    auto cursor = name.unit_list.begin();
    auto stop = name.unit_list.end();
    if (cursor != stop) {
        builder << to_string_cdba(*cursor);
        ++cursor;
    }
    for (; cursor != stop; ++cursor) {
        builder << ',' << to_string_cdba(*cursor);
    }
    return builder.str();
}

} // namespace cbag
