/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <fmt/format.h>

#include <boost/range/join.hpp>
#include <boost/variant.hpp>

#include <cbag/database/cellviews.h>
#include <cbag/database/figures.h>
#include <cbag/netlist.h>
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

NetlistBuilder::LineBuilder &operator<<(NetlistBuilder::LineBuilder &builder,
                                        const std::string &token) {
    builder.tokens.push_back(token);
    return builder;
}

NetlistBuilder::LineBuilder &operator<<(NetlistBuilder::LineBuilder &builder,
                                        std::string &&token) {
    builder.tokens.push_back(token);
    return builder;
}

std::ofstream &operator<<(std::ofstream &stream,
                          const NetlistBuilder::LineBuilder &b) {
    size_t num_tokens = b.tokens.size();
    if (num_tokens == 0) {
        return stream;
    }
    stream << b.tokens[0];
    size_t cur_col = b.tokens[0].size();
    for (size_t idx = 1; idx < num_tokens; ++idx) {
        size_t n = b.tokens[idx].size();
        size_t cur_len = (b.break_before) ? n + 3 : n + 1;
        if (cur_col + cur_len <= b.ncol) {
            stream << ' ' << b.tokens[idx];
            cur_col += n + 1;
        } else {
            // line break
            if (b.break_before) {
                stream << ' ' << b.cnt_char << std::endl << b.tokens[idx];
                cur_col = n;
            } else {
                stream << std::endl << b.cnt_char << ' ' << b.tokens[idx];
                cur_col = n + 2;
            }
        }
    }
    stream << std::endl;
    return stream;
}

NetlistBuilder::NetlistBuilder(const char *fname)
    : out_file(fname, std::ios_base::out) {}

void NetlistBuilder::init() { write_header(); }

void NetlistBuilder::build() {
    write_end();
    out_file.close();
}

void NetlistBuilder::add_cellview(const std::string &name, SchCellView *cv,
                                  const netlist_map_t &cell_map) {
    write_cv_header(name, cv->in_terms, cv->out_terms, cv->io_terms);
    for (auto const &p : cv->instances) {
        write_instance(p.first, p.second, cell_map);
    }
    write_cv_end(name);
}

void NetlistBuilder::write_instance(const std::string &name,
                                    const Instance &inst,
                                    const netlist_map_t &cell_map) {
    auto libmap_iter = cell_map.find(inst.lib_name);
    if (libmap_iter == cell_map.end()) {
        throw std::invalid_argument(fmt::format(
            "Cannot find library {} in netlist map.", inst.lib_name));
    }
    auto cellmap_iter = libmap_iter->second.find(inst.cell_name);
    if (cellmap_iter == libmap_iter->second.end()) {
        throw std::invalid_argument(
            fmt::format("Cannot find cell {}__{} in netlist map.",
                        inst.lib_name, inst.cell_name));
    }

    write_instance_helper(name, inst, cellmap_iter->second);
}

CDLBuilder::CDLBuilder(const char *fname) : NetlistBuilder(fname) {
    write_header();
}

void CDLBuilder::write_header() {
    out_file << ".PARAM" << std::endl;
    out_file << std::endl;
}

void CDLBuilder::write_end() {}

void CDLBuilder::write_cv_header(const std::string &name,
                                 const term_t &in_terms,
                                 const term_t &out_terms,
                                 const term_t &io_terms) {
    LineBuilder b(ncol, cnt_char, break_before);
    b << ".SUBCKT";
    b << name;
    auto tmp_range = boost::join(in_terms, out_terms);
    for (auto const &pair : boost::join(tmp_range, io_terms)) {
        spirit::ast::name ast = parse_cdba_name(pair.first);
        for (auto const &bit : ast) {
            b << to_string_cdba(bit);
        }
    }

    out_file << b;
}

void CDLBuilder::write_cv_end(const std::string &name) {
    out_file << ".ENDS" << std::endl;
}

// Boost visitor for recording parameter values
class write_param_visitor : public boost::static_visitor<> {
  public:
    write_param_visitor(NetlistBuilder::LineBuilder *ptr,
                        const std::string *key)
        : ptr(ptr), key(key) {}

    // only support writing string values
    void operator()(const std::string &v) const {
        (*ptr) << fmt::format("{}={}", *key, v);
    }
    void operator()(const int32_t &v) const {}
    void operator()(const double &v) const {}
    void operator()(const bool &v) const {}
    void operator()(const Time &v) const {}
    void operator()(const Binary &v) const {}

  private:
    NetlistBuilder::LineBuilder *ptr;
    const std::string *key;
};

void CDLBuilder::write_instance_helper(const std::string &name,
                                       const Instance &inst,
                                       const SchCellViewInfo &info) {
    LineBuilder b(ncol, cnt_char, break_before);

    // <name> <net1> <net2> ... <cell name> <par1>=<val1> ...
    // write instance name
    b << name;

    // write instance connections
    auto tmp_range = boost::join(info.in_terms, info.out_terms);
    for (auto const &term : boost::join(tmp_range, info.io_terms)) {
        auto term_iter = inst.connections.find(term);
        if (term_iter == inst.connections.end()) {
            throw std::invalid_argument(
                fmt::format("Cannot find net {} in cellview {}__{}", term,
                            inst.lib_name, inst.cell_name));
        }
        b << term_iter->second;
    }

    // write instance cell name
    b << inst.cell_name;

    // get default parameter values
    ParamMap par_map(info.props);
    // update with instance parameters
    for (auto const &pair : inst.params) {
        par_map[pair.first] = pair.second;
    }
    // write instance parameters
    for (auto const &pair : par_map) {
        boost::apply_visitor(write_param_visitor(&b, &(pair.first)),
                             pair.second);
    }

    out_file << b;
}

std::unique_ptr<NetlistBuilder>
make_netlist_builder(const char *fname, const std::string &format) {
    if (format == "cdl") {
        return std::make_unique<CDLBuilder>(fname);
    } else {
        throw std::invalid_argument(
            fmt::format("Unrecognized netlist format: {}", format));
    }
}

} // namespace cbag
