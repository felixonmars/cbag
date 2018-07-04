//
// Created by erichang on 7/3/18.
//

#include <boost/functional/hash.hpp>

#include <cbag/common.h>

namespace cbag {

    bool Name::operator<(const Name &other) const {
        unsigned long this_size = unit_list.size();
        unsigned long that_size = other.unit_list.size();
        if (this_size < that_size) {
            return true;
        } else if (this_size == that_size) {
            for (unsigned long idx = 0; idx < this_size; idx++) {
                if (unit_list[idx] < other.unit_list[idx]) {
                    return true;
                } else if (other.unit_list[idx] < unit_list[idx]) {
                    return false;
                }
            }
            return false;
        } else {
            return false;
        }
    }

    Name NameFormatter::get_name(const std::string &name_str) {

        boost::char_separator<char> sep(std::string(1, delim).c_str());
        typedef boost::tokenizer<boost::char_separator<char> > Tok;
        Tok tok(name_str, sep);

        // fill in names
        Name ans;
        for (Tok::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
            ans.unit_list.push_back(get_name_unit(*beg));
        }
        return ans;
    }

    NameUnit NameFormatter::get_name_unit(const std::string &name_str) {
        if (name_str.find(delim) != std::string::npos) {
            // delimiter found, raise error
            std::ostringstream stream;
            stream << "Cannot parse NameUnit from string: " << name_str;
            throw std::invalid_argument(stream.str());
        }
        unsigned long vstart_idx = name_str.find(vec_start);
        if (vstart_idx != std::string::npos) {
            // vector start found
            if (name_str.find(vec_stop) != name_str.size() - 1) {
                // vector stop not at the end of the string
                std::ostringstream stream;
                stream << "Cannot parse NameUnit from string: " << name_str;
                throw std::invalid_argument(stream.str());
            }
            return NameUnit(name_str.substr(0, vstart_idx),
                            get_range(name_str.substr(vstart_idx + 1, name_str.size() - vstart_idx - 2)));
        } else {
            // no vector start, assume this is scalar name
            return NameUnit(name_str);
        }
    }

    Range NameFormatter::get_range(const std::string &range_str) {

        boost::char_separator<char> sep(&vec_delim);
        typedef boost::tokenizer<boost::char_separator<char> > Tok;
        Tok tok(range_str, sep);
        uint32_t cnt = 0;
        uint32_t r_start = 0;
        uint32_t r_stop = 0;
        uint32_t r_step = 0;
        for (Tok::iterator beg = tok.begin(); beg != tok.end(); ++beg, ++cnt) {
            switch (cnt) {
                case 0:
                    r_stop = r_start = static_cast<uint32_t>(std::stoul(*beg));
                    r_step = 1;
                    break;
                case 1:
                    r_stop = static_cast<uint32_t>(std::stoul(*beg));
                    break;
                case 2:
                    r_step = static_cast<uint32_t>(std::stoul(*beg));
                    break;
                default: {
                    std::ostringstream stream;
                    stream << "Cannot parse Range from string: " << range_str;
                    throw std::invalid_argument(stream.str());
                }
            }
        }

        if (r_step == 0) {
            std::ostringstream stream;
            stream << "Cannot parse Range from string: " << range_str;
            throw std::invalid_argument(stream.str());
        }


        uint32_t n = (r_stop >= r_start) ? (r_stop - r_start + r_step) / r_step : (r_start - r_stop + r_step) / r_step;

        // return answer
        return {r_start, r_start + n * r_step, r_step};
    }

}

namespace std {

    // define hash function for NameUnit
    template<>
    struct hash<cbag::NameBit> {
        size_t operator()(const cbag::NameBit &v) const {

            size_t seed = 0;
            boost::hash_combine(seed, v.name);
            boost::hash_combine(seed, v.index);

            return seed;
        }
    };

}