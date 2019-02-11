
#ifndef CBAG_NETLIST_CORE_H
#define CBAG_NETLIST_CORE_H

#include <fstream>
#include <string>
#include <type_traits>
#include <vector>

#include <cbag/logging/logging.h>

#include <fmt/core.h>

#include <cbag/netlist/netlist_map_t.h>

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
                                const sch::cellview_info &info, bool shell) {}

    static void write_cv_end(type &stream, const std::string &name) {}

    static void write_instance(type &stream, const std::string &name, const sch::instance &inst,
                               const sch::cellview_info &info) {}

    static void append_netlist(type &stream, const std::string &netlist) {}
};

} // namespace traits

template <typename Stream, typename = typename traits::nstream<Stream>::type>
void add_cellview(Stream &stream, const std::string &name, const sch::cellview &cv,
                  const sch::cellview_info &info, const netlist_map_t &cell_map, bool shell) {
    traits::nstream<Stream>::write_cv_header(stream, name, info, shell);
    if (!shell) {
        for (auto const &p : cv.instances) {
            const sch::instance &inst = *(p.second);

            // get instance master's information object
            auto libmap_iter = cell_map.find(inst.lib_name);
            if (libmap_iter == cell_map.end()) {
                throw std::invalid_argument(
                    fmt::format("Cannot find library {} in netlist map for cell {}.  "
                                "Check your primitives definition file.",
                                inst.lib_name, inst.cell_name));
            }
            auto cellmap_iter = libmap_iter->second.find(inst.cell_name);
            if (cellmap_iter == libmap_iter->second.end()) {
                throw std::invalid_argument(fmt::format("Cannot find cell {}__{} in netlist map.",
                                                        inst.lib_name, inst.cell_name));
            }

            // Only write instance if the name is not empty
            if (!cellmap_iter->second.cell_name.empty()) {
                traits::nstream<Stream>::write_instance(stream, p.first, inst,
                                                        cellmap_iter->second);
            }
        }
    }
    traits::nstream<Stream>::write_cv_end(stream, name);
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
