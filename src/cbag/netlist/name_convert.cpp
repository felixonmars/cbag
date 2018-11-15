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
#include <cbag/spirit/name_unit_def.h>
#include <cbag/spirit/parsers.h>
#include <cbag/util/overload.h>

namespace cbag {

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

std::string to_string_cdba(const spirit::ast::name_unit &nu) {
    return fmt::format("{}{}", nu.base, to_string_cdba(nu.idx_range));
}

struct str_visitor : boost::static_visitor<std::string> {
    uint32_t mult;

    str_visitor(uint32_t mult) : mult(mult) {}

    std::string operator()(const spirit::ast::name_unit &arg) const {
        if (mult == 1)
            return to_string_cdba(arg);
        return fmt::format("<*{}>{}", mult, to_string_cdba(arg));
    }
    std::string operator()(const spirit::ast::name &arg) const {
        if (mult == 1)
            return to_string_cdba(arg);
        return fmt::format("<*{}>({})", mult, to_string_cdba(arg));
    }
};

std::string to_string_cdba(const spirit::ast::name_rep &nr) {
    return boost::apply_visitor(str_visitor(nr.mult), nr.data);
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
