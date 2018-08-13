/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_CDL_H
#define CBAG_NETLIST_CDL_H

#include <cbag/netlist/netlist.h>


namespace cbag {

class CDLBuilder : public NetlistBuilder {
  public:
    explicit CDLBuilder(const char *fname) : NetlistBuilder(fname) {}

  private:
    void write_header(const std::vector<std::string> &inc_list) override;

    void write_end() override;

    void write_cv_header(const std::string &name, const term_t &in_terms,
                         const term_t &out_terms,
                         const term_t &io_terms) override;

    void write_cv_end(const std::string &name) override;

    void write_instance_helper(const std::string &name, const Instance &inst,
                               const SchCellViewInfo &info) override;

    static const size_t ncol = 80;
    static const char cnt_char = '+';
    static const bool break_before = false;
};

} // namespace cbag

#endif // CBAG_NETLIST_CDL_H
