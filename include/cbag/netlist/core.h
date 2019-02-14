
#ifndef CBAG_NETLIST_CORE_H
#define CBAG_NETLIST_CORE_H

#include <fstream>
#include <string>
#include <type_traits>
#include <vector>

#include <cbag/logging/logging.h>

#include <fmt/core.h>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>

namespace cbag {

// forward declaration
struct time_struct;
struct binary_t;

namespace netlist {

// netlister base class

namespace traits {

template <typename T> struct nstream {
    using type = T;

    static void close(type &stream) {}

    static void write_header(type &stream, const std::vector<std::string> &inc_list, bool shell) {}

    static void write_end(type &stream) {}

    static void write_cv_header(type &stream, const std::string &name,
                                const sch::cellview_info &info, bool shell, bool write_subckt) {}

    static void write_cv_end(type &stream, const std::string &name, bool write_subckt) {}

    static void write_instance(type &stream, const std::string &name, const sch::instance &inst,
                               const sch::cellview_info &info) {}

    static void append_netlist(type &stream, const std::string &netlist) {}
};

} // namespace traits

template <typename Stream, typename = typename traits::nstream<Stream>::type>
void add_cellview(Stream &stream, const std::string &name, const sch::cellview &cv,
                  const sch::cellview_info &info, const sch::netlist_map_t &cell_map, bool shell,
                  bool write_subckt) {
    traits::nstream<Stream>::write_cv_header(stream, name, info, shell, write_subckt);
    if (!shell) {
        for (auto const &p : cv.instances) {
            const sch::instance &inst = *(p.second);

            auto &cv_info = sch::get_cv_info(cell_map, inst.lib_name, inst.cell_name);
            // Only write instance if the name is not empty
            if (!cv_info.cell_name.empty()) {
                traits::nstream<Stream>::write_instance(stream, p.first, inst, cv_info);
            }
        }
    }
    traits::nstream<Stream>::write_cv_end(stream, name, write_subckt);
}

template <typename Stream, typename = typename traits::nstream<Stream>::type>
void add_cellview(Stream &stream, const std::string &netlist) {
    traits::nstream<Stream>::append_netlist(stream, netlist);
}

class nstream_file {
  public:
    std::ofstream out_file;

    explicit nstream_file(const std::string &fname);

    void close();
};

template <class OutIter> class write_param_visitor {
  private:
    OutIter &iter_;
    const std::string &key_;

  public:
    write_param_visitor(OutIter &iter, const std::string &key) : iter_(iter), key_(key) {}

    void operator()(const std::string &v) const { *iter_ = fmt::format("{}={}", key_, v); }
    void operator()(const int_fast32_t &v) const {
        auto logger = cbag::get_cbag_logger();
        logger->warn("integer parameter, do nothing.");
    }
    void operator()(const double_t &v) const {
        auto logger = cbag::get_cbag_logger();
        logger->warn("integer parameter, do nothing.");
    }
    void operator()(const bool &v) const {
        auto logger = cbag::get_cbag_logger();
        logger->warn("integer parameter, do nothing.");
    }
    void operator()(const time_struct &v) const {
        auto logger = cbag::get_cbag_logger();
        logger->warn("integer parameter, do nothing.");
    }
    void operator()(const binary_t &v) const {
        auto logger = cbag::get_cbag_logger();
        logger->warn("integer parameter, do nothing.");
    }
};

} // namespace netlist
} // namespace cbag

#endif // CBAG_NETLIST_NETLIST_H
