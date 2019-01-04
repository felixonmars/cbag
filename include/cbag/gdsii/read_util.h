
#ifndef CBAG_GDSII_READ_UTIL_H
#define CBAG_GDSII_READ_UTIL_H

#include <fstream>
#include <string>
#include <tuple>
#include <unordered_map>

#include <cbag/common/layer_t.h>
#include <cbag/common/transformation.h>
#include <cbag/gdsii/record_type.h>
#include <cbag/gdsii/typedefs.h>
#include <cbag/layout/polygon.h>
#include <cbag/logging/logging.h>

namespace cbag {

class box_t;

namespace layout {
class instance;
class cellview;
} // namespace layout

namespace gdsii {

std::tuple<record_type, std::size_t> read_record_header(std::ifstream &stream);

void read_header(spdlog::logger &logger, std::ifstream &stream);

void read_lib_begin(spdlog::logger &logger, std::ifstream &stream);

std::string read_lib_name(spdlog::logger &logger, std::ifstream &stream);

void read_units(spdlog::logger &logger, std::ifstream &stream);

std::string read_struct_name(spdlog::logger &logger, std::ifstream &stream);

std::tuple<gds_layer_t, transformation, std::string> read_text(spdlog::logger &logger,
                                                               std::ifstream &stream);

std::tuple<gds_layer_t, layout::polygon> read_box(spdlog::logger &logger, std::ifstream &stream);

std::tuple<gds_layer_t, layout::polygon> read_boundary(spdlog::logger &logger,
                                                       std::ifstream &stream);

layout::instance
read_instance(spdlog::logger &logger, std::ifstream &stream, std::size_t cnt,
              const std::unordered_map<std::string, layout::cellview *> &master_map);

layout::instance
read_arr_instance(spdlog::logger &logger, std::ifstream &stream, std::size_t cnt,
                  const std::unordered_map<std::string, layout::cellview *> &master_map);

} // namespace gdsii
} // namespace cbag

#endif
