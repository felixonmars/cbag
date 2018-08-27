
#include <fmt/format.h>

#include <cbag/layout/cellview.h>
#include <cbag/util/binary_iterator.h>

namespace cbag {
namespace layout {

std::string cellview::add_instance(instance inst, const char *name) {
    std::string inst_name;
    if (name == nullptr) {
        inst_name = get_inst_name();
    } else {
        inst_name = name;
        if (inst_map.find(inst_name) != inst_map.end()) {
            inst_name = get_inst_name();
        }
    }

    inst_map.emplace(inst_name, std::move(inst));
    return inst_name;
}

std::string cellview::get_inst_name() {
    cbag::util::binary_iterator<uint32_t> iter(inst_name_cnt, std::optional<uint32_t>());
    while (iter.has_next()) {
        if (inst_map.find(fmt::format("X{d}", *iter)) == inst_map.end()) {
            iter.save();
            iter.down();
        } else {
            iter.up();
        }
    }

    inst_name_cnt = *(iter.get_save());
    return fmt::format("X{d}", inst_name_cnt);
}

} // namespace layout
} // namespace cbag
