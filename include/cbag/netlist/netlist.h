/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_NETLIST_H
#define CBAG_NETLIST_NETLIST_H

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <cbag/netlist/name_convert.h>

namespace cbag {

// forward declarations and type defs

struct Instance;
struct SchCellView;
struct SchCellViewInfo;
struct PinFigure;
using term_t = std::map<std::string, PinFigure>;
using lib_map_t = std::unordered_map<std::string, SchCellViewInfo>;
using netlist_map_t = std::unordered_map<std::string, lib_map_t>;

// netlister base class

class NetlistBuilder {
  public:
    /** A helper class for writing a line with column limit
     */
    class LineBuilder {
      public:
        LineBuilder(size_t ncol, char cnt_char, bool break_before, int tab_size);

        friend LineBuilder &operator<<(LineBuilder &builder, const std::string &token);

        friend LineBuilder &operator<<(LineBuilder &builder, std::string &&token);

        friend std::ofstream &operator<<(std::ofstream &stream, const LineBuilder &b);

      private:
        std::vector<std::string> tokens;
        size_t ncol;
        char cnt_char;
        bool break_before;
        int tab_size;
    };

    explicit NetlistBuilder(const char *fname);

    virtual void init(const std::vector<std::string> &inc_list, bool shell) = 0;

    void build();

    void add_cellview(const std::string &name, SchCellView *cv, const netlist_map_t &cell_map,
                      bool shell);

  protected:
    std::ofstream out_file;

    void write_instance(const std::string &name, const Instance &inst,
                        const netlist_map_t &cell_map);

  private:
    virtual void write_end() = 0;

    virtual void write_cv_header(const std::string &name, const term_t &in_terms,
                                 const term_t &out_terms, const term_t &io_terms) = 0;

    virtual void write_cv_end(const std::string &name) = 0;

    virtual void write_instance_helper(const std::string &name, const Instance &inst,
                                       const SchCellViewInfo &info) = 0;
};

// forward declaration
struct Time;
struct Binary;

class write_param_visitor {
  public:
    write_param_visitor(NetlistBuilder::LineBuilder *ptr, const std::string *key);

    void operator()(const std::string &v) const;
    void operator()(const int32_t &v) const;
    void operator()(const double &v) const;
    void operator()(const bool &v) const;
    void operator()(const Time &v) const;
    void operator()(const Binary &v) const;

  private:
    NetlistBuilder::LineBuilder *ptr;
    const std::string *key;
};

} // namespace cbag

#endif // CBAG_NETLIST_NETLIST_H
