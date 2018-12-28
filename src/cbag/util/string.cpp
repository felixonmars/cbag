#include <cbag/util/string.h>

namespace cbag {
namespace util {

token_iterator::token_iterator(const std::string &text, const char *separators) {
    boost::char_separator<char> sep(separators);
    tokens = std::make_unique<tok_container_t>(text, sep);
    cur = tokens->begin();
    end = tokens->end();
}

bool token_iterator::has_next() const { return cur != end; }

std::string token_iterator::get_next() {
    auto ans = *cur;
    ++cur;
    return ans;
}

} // namespace util
} // namespace cbag
