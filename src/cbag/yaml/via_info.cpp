#include <type_traits>

#include <cbag/logging/logging.h>

#include <cbag/common/typedefs.h>
#include <cbag/yaml/common.h>
#include <cbag/yaml/vector.h>
#include <cbag/yaml/via_info.h>

namespace YAML {

bool convert<cbag::layout::venc_data>::decode(const Node &node, cbag::layout::venc_data &rhs) {
    auto logger = cbag::get_cbag_logger();

    if (!node.IsSequence() || node.size() != 2) {
        logger->warn("cbag::layout::vend_data YAML decode: not a sequence or size != 2.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }

    try {
        rhs.width = cbagyaml::get_int<cbag::offset_t>(node[0]);

        rhs.enc_list = node[1].as<std::decay_t<decltype(rhs.enc_list)>>();
    } catch (...) {
        logger->warn("cbag::layout::venc_data YAML decode exception.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    return true;
}

bool convert<cbag::layout::via_info>::decode(const Node &node, cbag::layout::via_info &rhs) {
    auto logger = cbag::get_cbag_logger();
    try {
        auto cut_type = node["name"].as<std::string>();
        auto cut_dim = node["dim"].as<cbag::vector>();
        auto sp = node["sp"].as<cbag::vector>();
        auto sp2_list = node["sp2"].as<std::vector<cbag::vector>>();
        auto sp3_list = node["sp3"].as<std::vector<cbag::vector>>();
        std::array<cbag::layout::venc_info, 2> enc_list = {
            node["bot_enc"].as<cbag::layout::venc_info>(),
            node["top_enc"].as<cbag::layout::venc_info>()};

        rhs = cbag::layout::via_info(std::move(cut_type), std::move(cut_dim), std::move(sp),
                                     std::move(sp2_list), std::move(sp3_list), std::move(enc_list));
    } catch (...) {
        logger->warn("cbag::layout::via_info YAML decode exception.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    return true;
}

} // namespace YAML
