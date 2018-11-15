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
    parse(source, spirit::name_rep(), ast);
    return ast;
}

spirit::ast::name parse_cdba_name(const std::string &source) {
    spirit::ast::name ast;
    parse(source, spirit::name(), ast);
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

std::string to_string_cdba(const spirit::ast::name_rep &nr) {
    if (nr.mult == 1) {
        return fmt::format("{}{}", nr.data.base, to_string_cdba(nr.data.idx_range));
    } else {
        return fmt::format("<*{}>{}{}", nr.mult, nr.data.base, to_string_cdba(nr.data.idx_range));
    }
}

std::string to_string_cdba(const spirit::ast::name &name) {
    std::ostringstream builder;
    auto cursor = name.rep_list.begin();
    auto stop = name.rep_list.end();
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
