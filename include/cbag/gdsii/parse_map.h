
#ifndef CBAG_GDSII_PARSE_MAP_H
#define CBAG_GDSII_PARSE_MAP_H

#include <string>

#include <cbag/util/io.h>
#include <cbag/util/string.h>

namespace cbag {
namespace gdsii {

void check_has_next(const util::token_iterator &iter, const std::string &fname);

uint16_t to_int(std::string &&s, const std::string &fname);

template <typename F> void process_file(const std::string &fname, F fun) {
    auto file = util::open_file_read(fname);
    std::string line;
    while (std::getline(file, line)) {
        // ignore comments
        if (!line.empty() && line[0] != '#') {
            util::token_iterator iter(line, " \t");
            check_has_next(iter, fname);
            auto val1 = iter.get_next();
            check_has_next(iter, fname);
            auto val2 = iter.get_next();
            check_has_next(iter, fname);
            auto lay_id = to_int(iter.get_next(), fname);
            check_has_next(iter, fname);
            auto purp_id = to_int(iter.get_next(), fname);
            fun(val1, val2, lay_id, purp_id);
        }
    }
}

} // namespace gdsii
} // namespace cbag

#endif
