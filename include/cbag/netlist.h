/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_H
#define CBAG_NETLIST_H

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <cbag/spirit/ast.h>

namespace cbag {

// string <-> name conversion methods

spirit::ast::name_unit parse_cdba_name_unit(const std::string &source);

spirit::ast::name parse_cdba_name(const std::string &source);

spirit::ast::name parse_cdba_name(const char *source);

std::string to_string_cdba(const spirit::ast::name_bit &bit);

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
        LineBuilder(size_t ncol, char cnt_char, bool break_before)
            : tokens(), ncol(ncol), cnt_char(cnt_char),
              break_before(break_before) {}

        friend LineBuilder &operator<<(LineBuilder &builder,
                                       const std::string &token);

        friend LineBuilder &operator<<(LineBuilder &builder,
                                       std::string &&token);

        friend std::ofstream &operator<<(std::ofstream &stream,
                                         const LineBuilder &b);

      private:
        std::vector<std::string> tokens;
        size_t ncol;
        char cnt_char;
        bool break_before;
    };

    explicit NetlistBuilder(const char *fname);

    void init();

    void build();

    void add_cellview(const std::string &name, const SchCellView &cv,
                      const netlist_map_t &cell_map);

  protected:
    std::ofstream out_file;

    void write_instance(const std::string &name, const Instance &inst,
                        const netlist_map_t &cell_map);

  private:
    virtual void write_header() = 0;

    virtual void write_end() = 0;

    virtual void write_cv_header(const std::string &name,
                                 const term_t &in_terms,
                                 const term_t &out_terms,
                                 const term_t &io_terms) = 0;

    virtual void write_cv_end(const std::string &name) = 0;

    virtual void write_instance_helper(const std::string &name,
                                       const Instance &inst,
                                       const SchCellViewInfo &info) = 0;
};

// Spice netlister

class SpiceBuilder : public NetlistBuilder {
  public:
    explicit SpiceBuilder(const char *fname);

  private:
    void write_header() override;

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

std::unique_ptr<NetlistBuilder> make_netlist_builder(const char *fname,
                                                     const std::string &format);

} // namespace cbag

#endif // CBAG_NETLIST_H
