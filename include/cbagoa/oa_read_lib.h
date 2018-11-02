#ifndef CBAGOA_OA_READ_LIB_H
#define CBAGOA_OA_READ_LIB_H

#include <unordered_set>

#include <boost/functional/hash.hpp>

#include <fmt/format.h>

#include <oa/oaDesignDB.h>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/instance.h>

#include <cbagoa/oa_util.h>

namespace cbagoa {

using cell_key_t = std::pair<std::string, std::string>;
using str_map_t = std::unordered_map<std::string, std::string>;
using cell_set_t = std::unordered_set<cell_key_t, boost::hash<cell_key_t>>;

cbag::sch::cellview cell_to_yaml(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                                 spdlog::logger &logger, const std::string &lib_name,
                                 const std::string &cell_name, const std::string &sch_view,
                                 const std::string &root_path);

template <class OutIter>
void get_cells(const oa::oaNativeNS &ns_native, spdlog::logger &logger, const std::string &lib_name,
               OutIter out_iter) {
    try {
        oa::oaLib *lib_ptr = open_library_read(ns_native, lib_name);
        oa::oaIter<oa::oaCell> cell_iter(lib_ptr->getCells());
        oa::oaCell *cell_ptr;
        oa::oaString tmp_str;
        while ((cell_ptr = cell_iter.getNext()) != nullptr) {
            cell_ptr->getName(ns_native, tmp_str);
            out_iter = std::string(tmp_str);
        }
        lib_ptr->releaseAccess();
    } catch (...) {
        handle_oa_exceptions(logger);
    }
}

template <class OutIter>
void read_sch_helper(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                     spdlog::logger &logger, std::pair<std::string, std::string> &key,
                     const std::string &view_name, const str_map_t &yaml_path_map,
                     const std::unordered_set<std::string> &primitive_libs,
                     cell_set_t &exclude_cells, OutIter &out_iter) {
    // find root_path
    auto map_iter = yaml_path_map.find(key.first);
    if (map_iter == yaml_path_map.end())
        throw std::runtime_error(fmt::format(
            "Cannot find yaml path for library {}, is the library registered?", key.first));
    const std::string &root_path = map_iter->second;

    // write cellviews to yaml files
    cbag::sch::cellview sch_cv =
        cell_to_yaml(ns_native, ns, logger, key.first, key.second, view_name, root_path);

    // update cell_list and exclude_cells
    out_iter = key;
    exclude_cells.insert(std::move(key));

    // recurse
    auto exc_lib_end = primitive_libs.end();
    for (const auto &pair : sch_cv.instances) {
        std::pair<std::string, std::string> ikey(pair.second.lib_name, pair.second.cell_name);
        if (exclude_cells.find(ikey) == exclude_cells.end()) {
            // did not see this schematic master before
            if (primitive_libs.find(pair.second.lib_name) == exc_lib_end) {
                // non-primitive master, parse normally
                read_sch_helper(ns_native, ns, logger, ikey, view_name, yaml_path_map,
                                primitive_libs, exclude_cells, out_iter);
            }
        }
    }
}

template <class OutIter>
void read_sch_recursive(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                        spdlog::logger &logger, const std::string &lib_name,
                        const std::string &cell_name, const std::string &view_name,
                        const str_map_t &yaml_path_map,
                        const std::unordered_set<std::string> &primitive_libs, OutIter out_iter) {
    auto key = std::make_pair(lib_name, cell_name);
    cell_set_t exclude_cells;
    read_sch_helper(ns_native, ns, logger, key, view_name, yaml_path_map, primitive_libs,
                    exclude_cells, out_iter);
}

template <class OutIter> struct read_library_processor {
    const oa::oaNativeNS &ns_native;
    const oa::oaCdbaNS &ns;
    spdlog::logger &logger;
    const std::string &lib_name;
    const std::string &view_name;
    const str_map_t &yaml_path_map;
    const std::unordered_set<std::string> &primitive_libs;
    cell_set_t exclude_cells;
    OutIter &out_iter;

    read_library_processor(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                           spdlog::logger &logger, const std::string &lib_name,
                           const std::string &view_name, const str_map_t &yaml_path_map,
                           const std::unordered_set<std::string> &primitive_libs, OutIter &out_iter)
        : ns_native(ns_native), ns(ns), logger(logger), lib_name(lib_name), view_name(view_name),
          yaml_path_map(yaml_path_map), primitive_libs(primitive_libs), out_iter(out_iter) {}

    read_library_processor &operator=(std::string &&cell_name) {
        std::pair<std::string, std::string> key(lib_name, cell_name);
        read_sch_helper<OutIter>(ns_native, ns, logger, key, view_name, yaml_path_map,
                                 primitive_libs, exclude_cells, out_iter);
        return *this;
    }
};

template <class OutIter>
void read_library(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns, spdlog::logger &logger,
                  const std::string &lib_name, const std::string &view_name,
                  const str_map_t &yaml_path_map,
                  const std::unordered_set<std::string> &primitive_libs, OutIter out_iter) {
    read_library_processor proc(ns_native, ns, logger, lib_name, view_name, yaml_path_map,
                                primitive_libs, out_iter);
    get_cells(ns_native, logger, lib_name, proc);
}

} // namespace cbagoa

#endif
