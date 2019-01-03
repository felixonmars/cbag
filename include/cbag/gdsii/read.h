
#ifndef CBAG_GDSII_READ_H
#define CBAG_GDSII_READ_H

#include <string>

#include <cbag/logging/logging.h>

#include <cbag/util/io.h>

#include <cbag/gdsii/record_type.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/tech.h>

namespace cbag {
namespace gdsii {

std::tuple<record_type, std::size_t> read_record_header(std::ifstream &stream);

std::string read_gds_start(spdlog::logger &logger, std::ifstream &stream);

layout::cellview read_lay_cellview(spdlog::logger &logger, std::ifstream &stream,
                                   const std::string &lib_name, const layout::tech &t);

template <class OutIter>
void read_gds(const std::string &fname, const std::string &layer_map, const std::string &obj_map,
              const layout::tech &t, OutIter &&out_iter) {
    auto logger = get_cbag_logger();

    // get gds file stream
    auto stream = util::open_file_read(fname, true);
    auto lib_name = read_gds_start(*logger, stream);

    bool is_done = false;
    while (!is_done) {
        auto [rtype, rsize] = read_record_header(stream);
        switch (rtype) {
        case record_type::BGNSTR:
            stream.ignore(rsize);
            *out_iter = read_lay_cellview(*logger, stream, lib_name, t);
            ++out_iter;
            break;
        case record_type::ENDLIB:
            is_done = true;
            break;
        default:
            stream.close();
            throw std::runtime_error("Unrecognized GDS record type: " +
                                     std::to_string(static_cast<int>(rtype)));
        }
    }

    stream.close();
}

} // namespace gdsii
} // namespace cbag

#endif
