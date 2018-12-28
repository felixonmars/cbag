
#ifndef CBAG_GDSII_MAIN_H
#define CBAG_GDSII_MAIN_H

#include <string>
#include <unordered_map>

#include <boost/functional/hash.hpp>

#include <cbag/logging/logging.h>

#include <cbag/util/io.h>

#include <cbag/gdsii/typedefs.h>
#include <cbag/common/layer_t.h>
#include <cbag/layout/cellview_fwd.h>
#include <cbag/enum/boundary_type.h>

namespace cbag {
namespace gdsii {

using layer_map = std::unordered_map<layer_t, gds_layer_t, boost::hash<layer_t>>;
using boundary_map = std::unordered_map<boundary_type, gds_layer_t>;

class gds_lookup {
private:
    layer_map lay_map;
    boundary_map bnd_map;
public:
    gds_lookup(const std::string &lay_map_file, const std::string &obj_map_file);

    layer_map::const_iterator find_gds_layer(const layer_t &key) const;

    boundary_map::const_iterator find_gds_layer(boundary_type bnd_type) const;

    layer_map::const_iterator end_layer() const;

    boundary_map::const_iterator end_boundary() const;
};

std::vector<tval_t> get_gds_time();

void write_gds_start(spdlog::logger &logger, std::ofstream &stream, const std::string &lib_name,
                     double resolution, double user_unit, const std::vector<tval_t> &time_vec);

void write_gds_stop(spdlog::logger &logger, std::ofstream &stream);

void write_lay_cellview(spdlog::logger &logger, std::ofstream &stream, const std::string &cell_name,
                        const cbag::layout::cellview &cv,
                        const std::unordered_map<std::string, std::string> &rename_map,
                        const std::vector<tval_t> &time_vec, const gds_lookup &lookup);

template <class Vector>
void implement_gds(const std::string &fname, const std::string &lib_name,
                   const std::string &layer_map, const std::string &obj_map, double resolution,
                   double user_unit, const Vector &cv_list) {
    auto logger = get_cbag_logger();
    auto time_vec = get_gds_time();

    gds_lookup lookup(layer_map, obj_map);

    // get gds file stream
    auto stream = util::open_file_write(fname, true);
    write_gds_start(*logger, stream, lib_name, resolution, user_unit, time_vec);

    std::unordered_map<std::string, std::string> rename_map{};
    for (const auto &cv_info : cv_list) {
        auto &[cv_cell_name, cv_ptr] = cv_info;
        auto cell_name = cv_ptr->get_name();
        logger->info("Creating layout cell {}", cv_cell_name);
        write_lay_cellview(*logger, stream, cv_cell_name, *cv_ptr, rename_map, time_vec, lookup);
        logger->info("cell name {} maps to {}", cell_name, cv_cell_name);
        rename_map[cell_name] = cv_cell_name;
    }

    write_gds_stop(*logger, stream);
}

} // namespace gdsii
} // namespace cbag

#endif
