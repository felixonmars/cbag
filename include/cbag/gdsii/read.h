
#ifndef CBAG_GDSII_READ_H
#define CBAG_GDSII_READ_H

#include <string>

#include <cbag/logging/logging.h>

#include <cbag/util/io.h>

#include <cbag/layout/tech.h>

namespace cbag {
namespace gdsii {

std::string read_gds_start(spdlog::logger &logger, std::ifstream &stream);

template <class OutIter>
void read_lay_cellviews(spdlog::logger &logger, std::ifstream &stream, const std::string &lib_name,
                        const layout::tech &t, OutIter &&out_iter) {}

template <class OutIter>
void read_gds(const std::string &fname, const std::string &layer_map, const std::string &obj_map,
              const layout::tech &t, OutIter &&out_iter) {
    auto logger = get_cbag_logger();

    // get gds file stream
    auto stream = util::open_file_read(fname, true);
    auto lib_name = read_gds_start(*logger, stream);
    read_lay_cellviews(*logger, stream, lib_name, t, std::forward<OutIter>(out_iter));

    stream.close();
}

} // namespace gdsii
} // namespace cbag

#endif
