/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_VERILOG_H
#define CBAG_NETLIST_VERILOG_H

#include <cbag/netlist/netlist.h>

namespace cbag {

class verilog_builder : public netlist_builder {
  private:
    static const size_t ncol = 120;
    static const char cnt_char = '\0';
    static const bool break_before = true;
    static const int tab_size = 4;

  public:
    explicit verilog_builder(const char *fname);

    void init(const std::vector<std::string> &inc_list, bool shell) override;

  private:
    void write_end() override;

    void write_cv_header(const std::string &name, const sch::term_t &in_terms,
                         const sch::term_t &out_terms, const sch::term_t &io_terms) override;

    void write_cv_end(const std::string &name) override;

    void write_instance_helper(const std::string &name, const sch::instance &inst,
                               const sch::cellview_info &info) override;
};

} // namespace cbag

#endif // CBAG_NETLIST_VERILOG_H
