
#ifndef CBAG_NETLIST_CORE_H
#define CBAG_NETLIST_CORE_H

#include <fstream>
#include <string>
#include <type_traits>
#include <vector>

#include <fmt/core.h>

#include <cbag/netlist/netlist_map_t.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/term_t.h>
#include <cbag/spirit/namespace_info.h>

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
                                const sch::cellview_info &info) {}

    static void write_cv_end(type &stream, const std::string &name) {}

    static void write_instance(type &stream, const std::string &name, const sch::instance &inst,
                               const sch::cellview_info &info) {}
};

} // namespace traits

template <typename Stream, typename traits::nstream<Stream>::type * = nullptr>
void add_cellview(Stream &stream, const std::string &name, const sch::cellview &cv,
                  const sch::cellview_info &info, const netlist_map_t &cell_map, bool shell) {
    traits::nstream<Stream>::write_cv_header(stream, name, info);
    if (!shell) {
        for (auto const &p : cv.instances) {
            const sch::instance &inst = *(p.second);

            // get instance master's information object
            auto libmap_iter = cell_map.find(inst.lib_name);
            if (libmap_iter == cell_map.end()) {
                throw std::invalid_argument(
                    fmt::format("Cannot find library {} in netlist map for cell {}.", inst.lib_name,
                                inst.cell_name));
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

class lstream {
  private:
    std::vector<std::string> tokens;
    size_t ncol = 80;
    char cnt_char = ' ';
    bool break_before = false;
    int tab_size = 4;

  public:
    class back_inserter {
      private:
        lstream *stream_ = nullptr;

      public:
        explicit back_inserter(lstream *stream);
        back_inserter &operator*();
        back_inserter &operator=(std::string name);
    };

    lstream(size_t ncol, char cnt_char, bool break_before, int tab_size);

    back_inserter get_back_inserter();

    friend lstream &operator<<(lstream &builder, const std::string &token);

    friend lstream &operator<<(lstream &builder, std::string &&token);

    friend std::ofstream &operator<<(std::ofstream &stream, const lstream &b);
};

class nstream_file {
  public:
    std::ofstream out_file;
    spirit::namespace_info ns;

    explicit nstream_file(const std::string &fname, spirit::namespace_type ns_type);

    void close();
};

class write_param_visitor {
  private:
    lstream *ptr = nullptr;
    const std::string &key;

  public:
    write_param_visitor(lstream *ptr, const std::string &key);

    void operator()(const std::string &v) const;
    void operator()(const int32_t &v) const;
    void operator()(const double &v) const;
    void operator()(const bool &v) const;
    void operator()(const time_struct &v) const;
    void operator()(const binary_t &v) const;
};

} // namespace netlist
} // namespace cbag

#endif // CBAG_NETLIST_NETLIST_H
