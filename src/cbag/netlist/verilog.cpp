/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <boost/range/join.hpp>

#include <fmt/format.h>

#include <cbag/netlist/verilog.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/instance.h>
#include <cbag/schematic/pin_figure.h>

namespace cbag {
verilog_builder::verilog_builder(const char *fname) : netlist_builder(fname) {}

void verilog_builder::init(const std::vector<std::string> &inc_list, bool shell) {
    // TODO: Add actual implementation
}

void verilog_builder::write_end() {}

void verilog_builder::write_cv_header(const std::string &name, const sch::term_t &in_terms,
                                      const sch::term_t &out_terms, const sch::term_t &io_terms) {
    // write module declaration
    line_builder b(ncol, cnt_char, break_before, tab_size);
    b << "module";
    b << name;
    b << "(";
    auto tmp_range1 = boost::join(in_terms, out_terms);
    auto tmp_range2 = boost::join(tmp_range1, io_terms);
    auto ptr = tmp_range2.begin();
    auto const pend = tmp_range2.end();
    while (ptr != pend) {
        // has elements
        const std::string *str_ptr = &(ptr->first);
        ++ptr;
        if (ptr == pend) {
            // The current string is the last string
            b << *str_ptr;
        } else {
            // The current string is not the last string
            b << ((*str_ptr) + std::string(","));
        }
    }
    b << ");";

    out_file << b;
    out_file << std::endl;

    // write io type
    for (auto const &pair : in_terms) {
        out_file << "    input " << pair.first << ";" << std::endl;
    }
    for (auto const &pair : out_terms) {
        out_file << "    output " << pair.first << ";" << std::endl;
    }
    for (auto const &pair : io_terms) {
        out_file << "    inout " << pair.first << ";" << std::endl;
    }
    out_file << std::endl;
}

void verilog_builder::write_cv_end(const std::string &name) {
    out_file << "endmodule" << std::endl;
}

void verilog_builder::write_instance_helper(const std::string &name, const sch::instance &inst,
                                            const sch::cellview_info &info) {
    // TODO: add actual implementation
}

} // namespace cbag
