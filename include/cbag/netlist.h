/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_H
#define CBAG_NETLIST_H

#include <string>
#include <vector>
#include <map>
#include <fstream>

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
    struct PinFigure;
    using term_t = std::map<std::string, PinFigure>;

    // netlister base class

    class NetlistBuilder {
    public:
        /** A helper class for writing a line with column limit
         */
        class LineBuilder {
        public:
            LineBuilder(size_t ncol, char cnt_char, bool break_before)
                    : tokens(), ncol(ncol), cnt_char(cnt_char), break_before(break_before) {}

            friend LineBuilder &operator<<(LineBuilder &builder, const std::string &token);

            friend LineBuilder &operator<<(LineBuilder &builder, std::string &&token);

            friend std::ofstream &operator<<(std::ofstream &stream, const LineBuilder &b);

        private:
            std::vector<std::string> tokens;
            size_t ncol;
            char cnt_char;
            bool break_before;
        };

      explicit NetlistBuilder(const char *fname);

        void build();

        void add_cellview(const std::string &name, const SchCellView &cv);

    protected:
        std::ofstream out_file;
    private:
        virtual void write_header() = 0;

        virtual void write_end() = 0;

        virtual void write_cv_header(const std::string &name, const term_t &in_terms,
                                     const term_t &out_terms, const term_t &io_terms) = 0;

        virtual void write_cv_end(const std::string &name) = 0;

        virtual void write_instance(const std::string &name, const Instance &inst) = 0;
    };

    // Spice netlister

    class SpiceBuilder : NetlistBuilder {
    public:
      explicit SpiceBuilder(const char *fname);

    private:
        void write_header() override;

        void write_end() override;

        void write_cv_header(const std::string &name, const term_t &in_terms,
                             const term_t &out_terms, const term_t &io_terms) override;

        void write_cv_end(const std::string &name) override;

        void write_instance(const std::string &name, const Instance &inst) override;

        static const size_t ncol = 80;
        static const char cnt_char = '+';
        static const bool break_before = false;
    };
}


#endif //CBAG_NETLIST_H
