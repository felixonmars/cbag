#include <fmt/core.h>

#include <cbag/util/overload.h>

#include <cbag/gdsii/parse_map.h>
#include <cbag/gdsii/read.h>
#include <cbag/gdsii/read_util.h>

namespace cbag {
namespace gdsii {

gds_to_lay_map get_gds_to_lay_map(const std::string &fname, const layout::tech &tech) {
    gds_to_lay_map ans;

    process_file(fname, [&ans, &tech](const std::string &s1, const std::string &s2, uint16_t glay,
                                      uint16_t gpurp) {
        auto lay = tech.get_layer_id(s1);
        auto purp = tech.get_purpose_id(s2);
        if (lay && purp) {
            ans.emplace(std::make_pair(glay, gpurp), std::make_pair(*lay, *purp));
        }
    });

    return ans;
}

gds_to_bnd_map get_gds_to_bnd_map(const std::string &fname) {
    gds_to_bnd_map ans;

    process_file(fname,
                 [&ans](const std::string &s1, const std::string &s2, uint16_t lay, uint16_t purp) {
                     if (s1 == "Boundary") {
                         if (s2 == "PR") {
                             ans.emplace(std::make_pair(lay, purp), boundary_type::PR);
                         } else if (s2 == "snap") {
                             ans.emplace(std::make_pair(lay, purp), boundary_type::snap);
                         }
                     }
                 });

    return ans;
}

gds_rlookup::gds_rlookup() = default;

gds_rlookup::gds_rlookup(const std::string &lay_fname, const std::string &obj_fname,
                         const layout::tech &t)
    : lay_map(get_gds_to_lay_map(lay_fname, t)), bnd_map(get_gds_to_bnd_map(obj_fname)) {}

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

void add_object(spdlog::logger &logger, layout::cellview &ans, gds_layer_t &&gds_key,
                layout::polygon &&poly, const gds_rlookup &rmap) {
    auto map_val = rmap.get_mapping(gds_key);
    std::visit(
        overload{
            [&ans, &poly](layer_t k) { ans.make_geometry(std::move(k)).add_shape(poly, false); },
            [&ans, &poly](boundary_type k) {
                layout::boundary bnd{k};
                bnd.set(poly.begin(), poly.end());
                ans.add_object(std::move(bnd));
            },
            [&logger, &gds_key](bool k) {
                logger.warn("Cannot find mapping for GDS layer/purpose: ({}, {}), skipping.",
                            gds_key.first, gds_key.second);
            },

        },
        map_val);
}

std::tuple<std::string, std::unique_ptr<layout::cellview>>
read_lay_cellview(spdlog::logger &logger, std::ifstream &stream, const std::string &lib_name,
                  const layout::routing_grid &g, const gds_rlookup &rmap,
                  const std::unordered_map<std::string, layout::cellview *> &master_map) {
    auto cell_name = read_struct_name(logger, stream);

    logger.info("GDS cellview name: " + cell_name);

    auto cv_ptr = std::make_unique<layout::cellview>(&g, std::move(cell_name), geometry_mode::POLY);

    auto inst_cnt = static_cast<std::size_t>(0);
    while (true) {
        auto [rtype, rsize] = read_record_header(stream);
        switch (rtype) {
        case record_type::TEXT: {
            logger.info("Reading layout text.");
            auto [gds_key, xform, text] = read_text(logger, stream);
            cv_ptr->add_label(rmap.get_layer_t(gds_key), std::move(xform), std::move(text));
            break;
        }
        case record_type::SREF:
            logger.info("Reading layout instance.");
            cv_ptr->add_object(read_instance(logger, stream, inst_cnt, master_map));
            ++inst_cnt;
            break;
        case record_type::AREF:
            logger.info("Reading layout array instance.");
            cv_ptr->add_object(read_arr_instance(logger, stream, inst_cnt, master_map));
            ++inst_cnt;
            break;
        case record_type::BOX: {
            logger.info("Reading layout box.");
            auto [gds_key, poly] = read_box(logger, stream);
            add_object(logger, *cv_ptr, std::move(gds_key), std::move(poly), rmap);
            break;
        }
        case record_type::BOUNDARY: {
            logger.info("Reading layout boundary.");
            auto [gds_key, poly] = read_boundary(logger, stream);
            add_object(logger, *cv_ptr, std::move(gds_key), std::move(poly), rmap);
            break;
        }
        case record_type::ENDSTR:
            logger.info("Finish reading GDS cellview.");
            return {std::move(cell_name), std::move(cv_ptr)};
        default:
            throw std::runtime_error("Unsupported record type in GDS struct: " +
                                     std::to_string(static_cast<int>(rtype)));
        }
    }
}

} // namespace gdsii
} // namespace cbag
