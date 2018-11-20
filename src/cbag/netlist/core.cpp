/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <boost/filesystem.hpp>

#include <cbag/common/datatypes.h>
#include <cbag/netlist/core.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/instance.h>
#include <cbag/util/io.h>

namespace fs = boost::filesystem;

namespace cbag {
namespace netlist {

lstream::back_inserter::back_inserter(lstream *stream) : stream_(stream) {}

lstream::back_inserter &lstream::back_inserter::operator*() { return *this; }
lstream::back_inserter &lstream::back_inserter::operator=(std::string name) {
    (*stream_) << std::move(name);
    return *this;
}

lstream::lstream(const line_format *fmt_info) : fmt_info(fmt_info) {}

bool lstream::empty() const { return tokens.empty(); }

lstream::back_inserter lstream::get_back_inserter() { return back_inserter(this); }

lstream &lstream::append_last(const char *seq) {
    tokens.back().append(seq);
    return *this;
}

lstream &lstream::append_last(const std::string &seq) {
    tokens.back().append(seq);
    return *this;
}

std::ofstream &lstream::append_to(std::ofstream &stream, bool newline) const {
    size_t num_tokens = tokens.size();
    int tab_size = fmt_info->tab_size;
    if (num_tokens == 0) {
        return stream;
    }
    stream << tokens[0];
    size_t cur_col = tokens[0].size();
    for (size_t idx = 1; idx < num_tokens; ++idx) {
        size_t n = tokens[idx].size();
        size_t cur_len = (fmt_info->break_before) ? n + 3 : n + 1;
        if (cur_col + cur_len <= fmt_info->ncol) {
            stream << ' ' << tokens[idx];
            cur_col += n + 1;
        } else {
            // line break
            if (fmt_info->cnt_str.empty()) {
                // no line break character
                stream << std::endl;
                for (int cnt = 0; cnt < tab_size; ++cnt) {
                    stream << ' ';
                }
                stream << tokens[idx];
                cur_col = n + tab_size;
            } else if (fmt_info->break_before) {
                stream << ' ' << fmt_info->cnt_str << std::endl;
                for (int cnt = 0; cnt < tab_size; ++cnt) {
                    stream << ' ';
                }
                stream << tokens[idx];
                cur_col = n + tab_size;
            } else {
                stream << std::endl << fmt_info->cnt_str << ' ' << tokens[idx];
                cur_col = n + 2;
            }
        }
    }
    if (newline)
        stream << std::endl;
    return stream;
}

lstream &operator<<(lstream &builder, const std::string &token) {
    builder.tokens.push_back(token);
    return builder;
}

lstream &operator<<(lstream &builder, std::string &&token) {
    builder.tokens.push_back(token);
    return builder;
}

lstream &operator<<(lstream &builder, const std::vector<std::string> &tokens) {
    builder.tokens.insert(builder.tokens.end(), tokens.begin(), tokens.end());
    return builder;
}

std::ofstream &operator<<(std::ofstream &stream, const lstream &b) {
    return b.append_to(stream, true);
}

nstream_file::nstream_file(const std::string &fname, spirit::namespace_type ns_type,
                           line_format line_fmt)
    : out_file(util::open_file_write(fname)), ns(get_ns_info(ns_type)),
      line_fmt(std::move(line_fmt)) {}

lstream nstream_file::make_lstream() const { return lstream(&line_fmt); }

void nstream_file::close() { out_file.close(); }

} // namespace netlist
} // namespace cbag
