/** \file cbag.cpp
 *  \brief This implements the top level header file for cbag library.
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#include <cstring>
#include <fstream>
#include <memory>

#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include "spdlog/details/signal_handler.h"
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#include <cbag/database/yaml_cellviews.h>
#include <cbag/netlist/cdl.h>
#include <cbag/netlist/verilog.h>

namespace cbag {
void init_logging() {
    spdlog::installCrashHandler();

    constexpr uint32_t max_log_size = 1024 * 1024 * 10;
    constexpr uint32_t num_log_file = 3;
    spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>("cbag", "cbag.log", max_log_size,
                                                               num_log_file);
}

void to_file(const SchCellView &cv, const char *fname) {
    std::ofstream outfile(fname, std::ios_base::out);
    YAML::Node node(cv);
    YAML::Emitter emitter;
    emitter << node;
    outfile << emitter.c_str() << std::endl;
    outfile.close();
}

std::unique_ptr<NetlistBuilder> make_netlist_builder(const char *fname, const std::string &format) {
    if (format == "cdl") {
        return std::make_unique<CDLBuilder>(fname);
    } else if (format == "verilog") {
        return std::make_unique<VerilogBuilder>(fname);
    } else {
        throw std::invalid_argument(fmt::format("Unrecognized netlist format: {}", format));
    }
}

void write_netlist(const std::vector<SchCellView *> &cv_list,
                   const std::vector<std::string> &name_list, const char *cell_map,
                   const std::vector<std::string> &inc_list, const char *format, bool flat,
                   bool shell, const char *fname) {
    auto logger = spdlog::get("cbag");
    logger->info("Writing netlist file: {}", fname);
    logger->info("Parsing netlist cell map: {}", cell_map);
    YAML::Node n = YAML::LoadFile(std::string(cell_map));
    netlist_map_t netlist_map = n.as<netlist_map_t>();

    logger->info("Creating netlist builder for netlist format: {}", format);
    auto builder_ptr = make_netlist_builder(fname, std::string(format));
    builder_ptr->init(inc_list, shell);

    size_t num = cv_list.size();
    for (size_t idx = 0; idx < num; ++idx) {
        if (!shell) {
            // add this cellview to netlist
            logger->info("Netlisting cellview: {}", name_list[idx]);
            builder_ptr->add_cellview(name_list[idx], cv_list[idx], netlist_map, false);

            // add this cellview to netlist map
            logger->info("Adding cellview to netlist cell map");
            auto lib_map_iter = netlist_map.find(cv_list[idx]->lib_name);
            if (lib_map_iter == netlist_map.end()) {
                logger->info("Cannot find library {}, creating lib cell map",
                             cv_list[idx]->lib_name);
                lib_map_t new_lib_map;
                new_lib_map.emplace(cv_list[idx]->cell_name,
                                    cv_list[idx]->get_info(name_list[idx]));
                netlist_map.emplace(cv_list[idx]->lib_name, new_lib_map);
            } else {
                lib_map_iter->second.emplace(cv_list[idx]->cell_name,
                                             cv_list[idx]->get_info(name_list[idx]));
            }
        } else if (idx == num - 1) {
            // add this cellview to netlist
            logger->info("Netlisting cellview: {}", name_list[idx]);
            builder_ptr->add_cellview(name_list[idx], cv_list[idx], netlist_map, true);
        }
    }

    // build final netlist
    builder_ptr->build();
}

} // namespace cbag
