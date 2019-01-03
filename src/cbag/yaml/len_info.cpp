#include <type_traits>

#include <cbag/logging/logging.h>

#include <cbag/common/typedefs.h>
#include <cbag/yaml/int_array.h>
#include <cbag/yaml/len_info.h>

namespace YAML {

template <> struct convert<cbag::layout::max_len_data> : public convert_arr<cbag::offset_t, 3> {};

bool convert<cbag::layout::len_info>::decode(const Node &node, cbag::layout::len_info &rhs) {
    auto logger = cbag::get_cbag_logger();
    try {
        auto w_al_list = node["w_al_list"].as<std::vector<cbag::layout::max_len_data>>();
        auto md_al_list = node["md_al_list"].as<std::vector<cbag::layout::max_len_data>>();
        rhs = cbag::layout::len_info(std::move(w_al_list), std::move(md_al_list));
    } catch (...) {
        logger->warn("cbag::layout::len_info YAML decode exception.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    return true;
}

} // namespace YAML
