#include <cbag/layout/tech_util.h>

namespace cbag {
namespace layout {

layer_t get_layer_t(const tech &t, const std::string &layer, const std::string &purpose) {
    return {t.get_layer_id(layer), t.get_purpose_id(layer)};
}

} // namespace layout
} // namespace cbag
