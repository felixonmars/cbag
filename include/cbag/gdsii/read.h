
#ifndef CBAG_GDSII_READ_H
#define CBAG_GDSII_READ_H

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>

#include <boost/functional/hash.hpp>

#include <cbag/logging/logging.h>

#include <cbag/util/io.h>

#include <cbag/common/layer_t.h>
#include <cbag/enum/boundary_type.h>
#include <cbag/gdsii/record_type.h>
#include <cbag/gdsii/typedefs.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/tech.h>

namespace cbag {
namespace gdsii {

using gds_to_lay_map = std::unordered_map<gds_layer_t, layer_t, boost::hash<gds_layer_t>>;
using gds_to_bnd_map = std::unordered_map<gds_layer_t, boundary_type, boost::hash<gds_layer_t>>;

class gds_rlookup {
  private:
    gds_to_lay_map lay_map;
    gds_to_bnd_map bnd_map;

  public:
    gds_rlookup();

    gds_rlookup(const std::string &lay_fname, const std::string &obj_fname, const layout::tech &t);

    std::variant<layer_t, boundary_type, bool> get_mapping(gds_layer_t key) const;

    layer_t get_layer_t(gds_layer_t key) const;
};

std::tuple<record_type, std::size_t> read_record_header(std::ifstream &stream);

std::string read_gds_start(spdlog::logger &logger, std::ifstream &stream);

std::tuple<std::string, std::unique_ptr<layout::cellview>>
read_lay_cellview(spdlog::logger &logger, std::ifstream &stream, const std::string &lib_name,
                  const layout::tech &t, const gds_rlookup &rmap,
                  const std::unordered_map<std::string, layout::cellview *> &master_map);

template <class OutIter>
void read_gds(const std::string &fname, const std::string &layer_map, const std::string &obj_map,
              const layout::tech &t, OutIter &&out_iter) {
    auto log_ptr = get_cbag_logger();

    // get gds file stream
    log_ptr->info("Reading GDS file {}", fname);
    auto stream = util::open_file_read(fname, true);
    auto lib_name = read_gds_start(*log_ptr, stream);
    log_ptr->info("GDS library: {}", lib_name);

    bool is_done = false;
    gds_rlookup rmap(layer_map, obj_map, t);
    std::unordered_map<std::string, layout::cellview *> cv_map;
    while (!is_done) {
        auto [rtype, rsize] = read_record_header(stream);
        switch (rtype) {
        case record_type::BGNSTR: {
            log_ptr->info("Reading GDS cellview");
            stream.ignore(rsize);
            auto [cell_name, cv_ptr] =
                read_lay_cellview(*log_ptr, stream, lib_name, t, rmap, cv_map);

            cv_map.emplace(cell_name, cv_ptr.get());
            *out_iter = std::move(cv_ptr);
            ++out_iter;
            break;
        }
        case record_type::ENDLIB:
            log_ptr->info("Finish reading GDS file {}", fname);
            stream.close();
            return;
        default:
            throw std::runtime_error("Unrecognized GDS record type: " +
                                     std::to_string(static_cast<int>(rtype)));
        }
    }
}

} // namespace gdsii
} // namespace cbag

#endif
