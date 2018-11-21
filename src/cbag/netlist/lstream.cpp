/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <cbag/netlist/lstream.h>

namespace cbag {
namespace netlist {

lstream::back_inserter::back_inserter(lstream *stream) : stream_(stream) {}

lstream::back_inserter &lstream::back_inserter::operator*() { return *this; }
lstream::back_inserter &lstream::back_inserter::operator=(std::string name) {
    (*stream_) << std::move(name);
    return *this;
}

lstream::lstream() {}

bool lstream::empty() const { return tokens.empty(); }

lstream::back_inserter lstream::get_back_inserter() { return back_inserter(this); }

lstream &lstream::append_last(const char *seq) {
    tokens.back().append(seq);
    return *this;
}

lstream &lstream::append_last(const std::string &seq) {
    tokens.back().append(seq);
    return *this;
}

lstream &lstream::operator<<(const std::string &token) {
    tokens.push_back(token);
    return *this;
}

lstream &lstream::operator<<(std::string &&token) {
    tokens.push_back(std::move(token));
    return *this;
}

lstream &lstream::operator<<(const std::vector<std::string> &tokens_) {
    tokens.insert(tokens.end(), tokens_.begin(), tokens_.end());
    return *this;
}

lstream &lstream::operator<<(std::vector<std::string> &&tokens_) {
    tokens.insert(tokens.end(), std::make_move_iterator(tokens_.begin()),
                  std::make_move_iterator(tokens_.end()));
    return *this;
}

struct lstream::helper {
    static void to_file_helper(const lstream &line, std::ofstream &stream, bool newline,
                               int tab_size, bool break_before, size_t ncol, const char *cnt_str) {
        auto iter = line.tokens.begin();
        auto stop = line.tokens.end();
        if (iter == stop) {
            return;
        }
        stream << *iter;
        size_t cur_col = iter->size();
        ++iter;
        for (; iter != stop; ++iter) {
            size_t n = iter->size();
            size_t cur_len = (break_before) ? n + 3 : n + 1;
            if (cur_col + cur_len <= ncol) {
                stream << ' ' << *iter;
                cur_col += n + 1;
            } else {
                // line break
                if (cnt_str == nullptr) {
                    // no line break character
                    stream << std::endl;
                    for (int cnt = 0; cnt < tab_size; ++cnt) {
                        stream << ' ';
                    }
                    stream << *iter;
                    cur_col = n + tab_size;
                } else if (break_before) {
                    stream << ' ' << cnt_str << std::endl;
                    for (int cnt = 0; cnt < tab_size; ++cnt) {
                        stream << ' ';
                    }
                    stream << *iter;
                    cur_col = n + tab_size;
                } else {
                    stream << std::endl << cnt_str << ' ' << *iter;
                    cur_col = n + 2;
                }
            }
        }
        if (newline)
            stream << std::endl;
    }
};

std::ofstream &lstream::to_file(std::ofstream &stream, spirit::namespace_cdba, bool newline) const {
    helper::to_file_helper(*this, stream, newline, 0, false, 80, "+");
    return stream;
}
std::ofstream &lstream::to_file(std::ofstream &stream, spirit::namespace_cdl_cmd,
                                bool newline) const {
    helper::to_file_helper(*this, stream, newline, 0, false, 80, "*+");
    return stream;
}
std::ofstream &lstream::to_file(std::ofstream &stream, spirit::namespace_verilog,
                                bool newline) const {
    helper::to_file_helper(*this, stream, newline, 4, true, 120, nullptr);
    return stream;
}

} // namespace netlist
} // namespace cbag
