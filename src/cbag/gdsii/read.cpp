#include <fmt/core.h>

#include <cbag/gdsii/read.h>
#include <cbag/gdsii/read_util.h>

namespace cbag {
namespace gdsii {

gds_rlookup::gds_rlookup() = default;

gds_rlookup::gds_rlookup(const std::string &lay_fname, const std::string &obj_fname,
                         const layout::tech &t) {
    // TODO: implement this
}

std::variant<layer_t, boundary_type, bool> gds_rlookup::get_mapping(gds_layer_t key) const {
    auto iter = lay_map.find(key);
    if (iter != lay_map.end())
        return iter->second;

    auto iter2 = bnd_map.find(key);
    if (iter2 != bnd_map.end())
        return iter2->second;

    return false;
}

layer_t gds_rlookup::get_layer_t(gds_layer_t key) const {
    auto iter = lay_map.find(key);
    if (iter == lay_map.end())
        throw std::runtime_error(fmt::format("Cannot find mapping for GDS layer/purpose: ({}, {})",
                                             key.first, key.second));
    return iter->second;
}

std::string read_gds_start(spdlog::logger &logger, std::ifstream &stream) {
    read_header(logger, stream);
    read_lib_begin(logger, stream);
    auto ans = read_lib_name(logger, stream);
    read_units(logger, stream);
    return ans;
}

layout::cellview read_lay_cellview(spdlog::logger &logger, std::ifstream &stream,
                                   const std::string &lib_name, const layout::tech &t,
                                   const gds_rlookup &rmap) {
    auto cell_name = read_struct_name(logger, stream);
    layout::cellview ans{&t, std::move(cell_name), geometry_mode::POLY};

    while (true) {
        auto [rtype, rsize] = read_record_header(stream);
        switch (rtype) {
        case record_type::TEXT: {
            auto [gds_key, xform, text] = read_text(logger, stream, rsize);
            ans.add_label(rmap.get_layer_t(gds_key), std::move(xform), std::move(text));
            break;
        }
        case record_type::SREF:
            ans.add_object(read_instance(logger, stream, rsize));
            break;
        case record_type::AREF:
            ans.add_object(read_arr_instance(logger, stream, rsize));
            break;
        case record_type::BOX: {
            auto [gds_key, box] = read_box(logger, stream, rsize);
            auto map_val = rmap.get_mapping(gds_key);
            // TODO: finish this
            break;
        }
        case record_type::BOUNDARY: {
            auto [gds_key, poly] = read_boundary(logger, stream, rsize);
            auto map_val = rmap.get_mapping(gds_key);
            // TODO: finish this
            break;
        }
        case record_type::ENDSTR:
            return ans;
        default:
            throw std::runtime_error("Unsupported record type in GDS struct: " +
                                     std::to_string(static_cast<int>(rtype)));
        }
    }
}

} // namespace gdsii
} // namespace cbag
