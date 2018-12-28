#ifndef CBAG_UTIL_STRING_H
#define CBAG_UTIL_STRING_H

#include <memory>
#include <string>

#include <boost/tokenizer.hpp>

namespace cbag {
namespace util {

using tok_container_t = boost::tokenizer<boost::char_separator<char>>;
using tok_iter_t = tok_container_t::const_iterator;

class token_iterator {
  private:
    std::unique_ptr<tok_container_t> tokens;
    tok_iter_t cur;
    tok_iter_t end;

  public:
    token_iterator(const std::string &text, const char *separators);

    bool has_next() const;

    std::string get_next();
};

} // namespace util
// namespace util
} // namespace cbag
#endif
