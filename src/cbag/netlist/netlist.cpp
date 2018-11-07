/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <cbag/logging.h>

#include <cbag/common/datatypes.h>
#include <cbag/netlist/netlist.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/instance.h>

namespace cbag {

netlist_builder::line_builder::line_builder(size_t ncol, char cnt_char, bool break_before,
                                            int tab_size)
    : ncol(ncol), cnt_char(cnt_char), break_before(break_before), tab_size(tab_size) {}

netlist_builder::line_builder &operator<<(netlist_builder::line_builder &builder,
                                          const std::string &token) {
    builder.tokens.push_back(token);
    return builder;
}

netlist_builder::line_builder &operator<<(netlist_builder::line_builder &builder,
                                          std::string &&token) {
    builder.tokens.push_back(token);
    return builder;
}

std::ofstream &operator<<(std::ofstream &stream, const netlist_builder::line_builder &b) {
    size_t num_tokens = b.tokens.size();
    int tab_size = b.tab_size;
    if (num_tokens == 0) {
        return stream;
    }
    stream << b.tokens[0];
    size_t cur_col = b.tokens[0].size();
    for (size_t idx = 1; idx < num_tokens; ++idx) {
        size_t n = b.tokens[idx].size();
        size_t cur_len = (b.break_before) ? n + 3 : n + 1;
        if (cur_col + cur_len <= b.ncol) {
            stream << ' ' << b.tokens[idx];
            cur_col += n + 1;
        } else {
            // line break
            if (b.cnt_char == '\0') {
                // no line break character
                stream << std::endl;
                for (int cnt = 0; cnt < tab_size; ++cnt) {
                    stream << ' ';
                }
                stream << b.tokens[idx];
                cur_col = n + tab_size;
            } else if (b.break_before) {
                stream << ' ' << b.cnt_char << std::endl;
                for (int cnt = 0; cnt < tab_size; ++cnt) {
                    stream << ' ';
                }
                stream << b.tokens[idx];
                cur_col = n + tab_size;
            } else {
                stream << std::endl << b.cnt_char << ' ' << b.tokens[idx];
                cur_col = n + 2;
            }
        }
    }
    stream << std::endl;
    return stream;
}

netlist_builder::netlist_builder(const char *fname) : out_file(fname, std::ios_base::out) {}

void netlist_builder::build() {
    write_end();
    out_file.close();
}

void netlist_builder::add_cellview(const std::string &name, sch::cellview *cv,
                                   const sch::cellview_info &info, const netlist_map_t &cell_map,
                                   bool shell) {
    write_cv_header(name, info);
    if (!shell) {
        for (auto const &p : cv->instances) {
            write_instance(p.first, *(p.second), cell_map);
        }
    }
    write_cv_end(name);
    out_file << std::endl;
}

void netlist_builder::write_instance(const std::string &name, const sch::instance &inst,
                                     const netlist_map_t &cell_map) {
    auto libmap_iter = cell_map.find(inst.lib_name);
    if (libmap_iter == cell_map.end()) {
        throw std::invalid_argument(fmt::format(
            "Cannot find library {} in netlist map for cell {}.", inst.lib_name, inst.cell_name));
    }
    auto cellmap_iter = libmap_iter->second.find(inst.cell_name);
    if (cellmap_iter == libmap_iter->second.end()) {
        throw std::invalid_argument(
            fmt::format("Cannot find cell {}__{} in netlist map.", inst.lib_name, inst.cell_name));
    }

    // Only write instance if the name is not empty
    if (!cellmap_iter->second.cell_name.empty()) {
        write_instance_helper(name, inst, cellmap_iter->second);
    }
}

write_param_visitor::write_param_visitor(netlist_builder::line_builder *ptr, const std::string *key)
    : ptr(ptr), key(key) {}

void write_param_visitor::operator()(const std::string &v) const {
    (*ptr) << fmt::format("{}={}", *key, v);
}
void write_param_visitor::operator()(const int32_t &v) const {
    auto logger = spdlog::get("cbag");
    logger->warn("integer parameter, do nothing.");
}
void write_param_visitor::operator()(const double &v) const {
    auto logger = spdlog::get("cbag");
    logger->warn("double parameter, do nothing.");
}
void write_param_visitor::operator()(const bool &v) const {
    auto logger = spdlog::get("cbag");
    logger->warn("bool parameter, do nothing.");
}
void write_param_visitor::operator()(const time_struct &v) const {
    auto logger = spdlog::get("cbag");
    logger->warn("time parameter, do nothing.");
}
void write_param_visitor::operator()(const binary_t &v) const {
    auto logger = spdlog::get("cbag");
    logger->warn("binary parameter, do nothing.");
}

} // namespace cbag
