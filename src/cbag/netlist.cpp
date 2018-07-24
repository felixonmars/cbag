/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <fmt/format.h>

#include <cbag/database/cellviews.h>
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
        return fmt::format("{}<{}>", bit.base, bit.index);
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

void NetlistBuilder::build() {
    write_end();
    out_file.close();
}

void NetlistBuilder::add_cellview(const std::string &name,
                                  const SchCellView &cv) {
    write_cv_header(name, cv.in_terms, cv.out_terms, cv.io_terms);
    for (auto const &p : cv.instances) {
        write_instance(p.first, p.second);
    }
    write_cv_end(name);
}

SpiceBuilder::SpiceBuilder(const char *fname) : NetlistBuilder(fname) {
    write_header();
}

void SpiceBuilder::write_header() {
    out_file << ".PARAM" << std::endl;
    out_file << std::endl;
}

void SpiceBuilder::write_end() {}

void SpiceBuilder::write_cv_header(const std::string &name,
                                   const term_t &in_terms,
                                   const term_t &out_terms,
                                   const term_t &io_terms) {
    LineBuilder b(ncol, cnt_char, break_before);
    b << ".SUBCKT";
    b << name;
    for (auto const &pair : in_terms) {
        spirit::ast::name ast = parse_cdba_name(pair.first);
        for (auto const &bit : ast) {
            b << to_string_cdba(bit);
        }
    }
    for (auto const &pair : out_terms) {
        spirit::ast::name ast = parse_cdba_name(pair.first);
        for (auto const &bit : ast) {
            b << to_string_cdba(bit);
        }
    }
    for (auto const &pair : io_terms) {
        spirit::ast::name ast = parse_cdba_name(pair.first);
        for (auto const &bit : ast) {
            b << to_string_cdba(bit);
        }
    }

    out_file << b;
}

void SpiceBuilder::write_cv_end(const std::string &name) {
    out_file << ".ENDS" << std::endl;
}

void SpiceBuilder::write_instance(const std::string &name,
                                  const Instance &inst) {}

int foobar() {
    int a = 50;
    double b = 100;
    return a * b;
}

} // namespace cbag
