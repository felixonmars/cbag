/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <boost/filesystem.hpp>

#include <cbag/logging/logging.h>

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

lstream::lstream(size_t ncol, char cnt_char, bool break_before, int tab_size)
    : ncol(ncol), cnt_char(cnt_char), break_before(break_before), tab_size(tab_size) {}

lstream::back_inserter lstream::get_back_inserter() { return back_inserter(this); }

lstream &operator<<(lstream &builder, const std::string &token) {
    builder.tokens.push_back(token);
    return builder;
}

lstream &operator<<(lstream &builder, std::string &&token) {
    builder.tokens.push_back(token);
    return builder;
}

std::ofstream &operator<<(std::ofstream &stream, const lstream &b) {
    size_t num_tokens = b.tokens.size();
    int tab_size = b.tab_size;
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
            if (b.cnt_char == '\0') {
                // no line break character
                stream << std::endl;
                for (int cnt = 0; cnt < tab_size; ++cnt) {
                    stream << ' ';
                }
                stream << b.tokens[idx];
                cur_col = n + tab_size;
            } else if (b.break_before) {
                stream << ' ' << b.cnt_char << std::endl;
                for (int cnt = 0; cnt < tab_size; ++cnt) {
                    stream << ' ';
                }
                stream << b.tokens[idx];
                cur_col = n + tab_size;
            } else {
                stream << std::endl << b.cnt_char << ' ' << b.tokens[idx];
                cur_col = n + 2;
            }
        }
    }
    stream << std::endl;
    return stream;
}

nstream_file::nstream_file(const std::string &fname, spirit::namespace_type ns_type)
    : out_file(util::open_file_write(fname)), ns(get_ns_info(ns_type)) {}

void nstream_file::close() { out_file.close(); }

write_param_visitor::write_param_visitor(lstream *ptr, const std::string &key)
    : ptr(ptr), key(key) {}

void write_param_visitor::operator()(const std::string &v) const {
    (*ptr) << fmt::format("{}={}", key, v);
}
void write_param_visitor::operator()(const int32_t &v) const {
    auto logger = cbag::get_cbag_logger();
    logger->warn("integer parameter, do nothing.");
}
void write_param_visitor::operator()(const double &v) const {
    auto logger = cbag::get_cbag_logger();
    logger->warn("double parameter, do nothing.");
}
void write_param_visitor::operator()(const bool &v) const {
    auto logger = cbag::get_cbag_logger();
    logger->warn("bool parameter, do nothing.");
}
void write_param_visitor::operator()(const time_struct &v) const {
    auto logger = cbag::get_cbag_logger();
    logger->warn("time parameter, do nothing.");
}
void write_param_visitor::operator()(const binary_t &v) const {
    auto logger = cbag::get_cbag_logger();
    logger->warn("binary parameter, do nothing.");
}

} // namespace netlist
} // namespace cbag
