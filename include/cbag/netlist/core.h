
#ifndef CBAG_NETLIST_CORE_H
#define CBAG_NETLIST_CORE_H

#include <fstream>
#include <string>
#include <vector>

#include <cbag/netlist/netlist_map_t.h>
#include <cbag/schematic/term_t.h>

namespace cbag {

namespace sch {
struct cellview;
struct instance;
} // namespace sch

// forward declaration
struct time_struct;
struct binary_t;

// netlister base class

class netlist_builder {
  public:
    /** A helper class for writing a line with column limit
     */
    class line_builder {
      private:
        std::vector<std::string> tokens;
        size_t ncol = 80;
        char cnt_char = ' ';
        bool break_before = false;
        int tab_size = 4;

      public:
        line_builder(size_t ncol, char cnt_char, bool break_before, int tab_size);

        friend line_builder &operator<<(line_builder &builder, const std::string &token);

        friend line_builder &operator<<(line_builder &builder, std::string &&token);

        friend std::ofstream &operator<<(std::ofstream &stream, const line_builder &b);
    };

  protected:
    std::ofstream out_file;

  public:
    explicit netlist_builder(const std::string &fname);

    virtual void init(const std::vector<std::string> &inc_list, bool shell) = 0;

    void build();

    void add_cellview(const std::string &name, const sch::cellview &cv,
                      const sch::cellview_info &info, const netlist_map_t &cell_map, bool shell);

  protected:
    void write_instance(const std::string &name, const sch::instance &inst,
                        const netlist_map_t &cell_map);

  private:
    virtual void write_end() = 0;

    virtual void write_cv_header(const std::string &name, const sch::cellview_info &info) = 0;

    virtual void write_cv_end(const std::string &name) = 0;

    virtual void write_instance_helper(const std::string &name, const sch::instance &inst,
                                       const sch::cellview_info &info) = 0;
};

class write_param_visitor {
  private:
    netlist_builder::line_builder *ptr = nullptr;
    const std::string *key = nullptr;

  public:
    write_param_visitor(netlist_builder::line_builder *ptr, const std::string *key);

    void operator()(const std::string &v) const;
    void operator()(const int32_t &v) const;
    void operator()(const double &v) const;
    void operator()(const bool &v) const;
    void operator()(const time_struct &v) const;
    void operator()(const binary_t &v) const;
};

} // namespace cbag

#endif // CBAG_NETLIST_NETLIST_H