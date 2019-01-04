#include <cbag/gdsii/parse_map.h>

namespace cbag {
namespace gdsii {

void check_has_next(const util::token_iterator &iter, const std::string &fname) {
    if (!iter.has_next())
        throw std::runtime_error("Cannot parse file: " + fname);
}

uint16_t to_int(std::string &&s, const std::string &fname) {
    try {
        return static_cast<uint16_t>(std::stoi(std::move(s)));
    } catch (const std::invalid_argument &) {
        throw std::runtime_error("Cannot parse file: " + fname);
    } catch (const std::out_of_range &) {
        throw std::runtime_error("Cannot parse file: " + fname);
    }
}

} // namespace gdsii
} // namespace cbag
