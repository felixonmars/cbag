/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <boost/range/join.hpp>

#include <fmt/core.h>

#include <cbag/netlist/verilog.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/instance.h>

namespace cbag {
namespace netlist {

verilog_stream::verilog_stream(const std::string &fname)
    : nstream_file(fname, spirit::namespace_type::VERILOG) {}

lstream verilog_stream::make_lstream() { return {ncol, cnt_char, break_before, tab_size}; }

void traits::nstream<verilog_stream>::close(type &stream) { stream.close(); }

void traits::nstream<verilog_stream>::write_header(type &stream,
                                                   const std::vector<std::string> &inc_list,
                                                   bool shell) {
    // TODO: Add actual implementation
}

void traits::nstream<verilog_stream>::write_end(type &stream) {}

void traits::nstream<verilog_stream>::write_cv_header(type &stream, const std::string &name,
                                                      const sch::cellview_info &info) {
    // write module declaration
    lstream b = verilog_stream::make_lstream();
    b << "module";
    b << name;
    b << "(";
    auto tmp_range1 = boost::join(info.in_terms, info.out_terms);
    auto tmp_range2 = boost::join(tmp_range1, info.io_terms);
    auto ptr = tmp_range2.begin();
    auto const pend = tmp_range2.end();
    while (ptr != pend) {
        // has elements
        const std::string &str_val = *ptr;
        ++ptr;
        if (ptr == pend) {
            // The current string is the last string
            b << str_val;
        } else {
            // The current string is not the last string
            b << (str_val + std::string(","));
        }
    }
    b << ");";

    stream.out_file << b;
    stream.out_file << std::endl;

    // write io type
    for (auto const &name : info.in_terms) {
        stream.out_file << "    input " << name << ";" << std::endl;
    }
    for (auto const &name : info.out_terms) {
        stream.out_file << "    output " << name << ";" << std::endl;
    }
    for (auto const &name : info.io_terms) {
        stream.out_file << "    inout " << name << ";" << std::endl;
    }
    stream.out_file << std::endl;
}

void traits::nstream<verilog_stream>::write_cv_end(type &stream, const std::string &name) {
    stream.out_file << "endmodule" << std::endl << std::endl;
}

void traits::nstream<verilog_stream>::write_instance(type &stream, const std::string &name,
                                                     const sch::instance &inst,
                                                     const sch::cellview_info &info) {
    // TODO: add actual implementation
}

} // namespace netlist
} // namespace cbag
