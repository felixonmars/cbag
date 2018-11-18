/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_CDL_H
#define CBAG_NETLIST_CDL_H

#include <cbag/netlist/core.h>

namespace cbag {
namespace netlist {

class cdl_builder : public netlist_builder {
  private:
    static const size_t ncol = 80;
    static const char cnt_char = '+';
    static const bool break_before = false;
    static const int tab_size = 0;

  public:
    explicit cdl_builder(const std::string &fname);

    void init(const std::vector<std::string> &inc_list, bool shell) override;

  private:
    void write_end() override;

    void write_cv_header(const std::string &name, const sch::cellview_info &info) override;

    void write_cv_end(const std::string &name) override;

    void write_instance_helper(const std::string &name, const sch::instance &inst,
                               const sch::cellview_info &info) override;
};

} // namespace netlist
} // namespace cbag

#endif // CBAG_NETLIST_CDL_H
