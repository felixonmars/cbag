//
// Created by erichang on 7/3/18.
//

#include <boost/functional/hash.hpp>

#include <cbag/common.h>

namespace cbag {

    inline bool Range::operator==(const Range &other) const {
        return (start == other.start && stop == other.stop && step == other.step);
    }

    inline bool Range::operator<(const Range &other) const {
        return (step < other.step ||
                (step == other.step &&
                 (start < other.start ||
                  (start == other.start && stop < other.stop))));
    }

    inline bool NameUnit::operator==(const NameUnit &other) const {
        return (name == other.name && range == other.range);
    }

    inline bool NameUnit::operator<(const NameUnit &other) const {
        return (name < other.name || (name == other.name && range < other.range));
    }

    bool Name::operator<(const Name &other) const {
        unsigned long this_size = unit_list.size();
        unsigned long that_size = other.unit_list.size();
        if (this_size < that_size) {
            return true;
        } else if (this_size == that_size) {
            auto it1 = unit_list.begin();
            auto it2 = other.unit_list.begin();
            for (; it1 != unit_list.end(); ++it1, ++it2) {
                if (*it1 < *it2) {
                    return true;
                } else if (*it2 < *it1) {
                    return false;
                }
            }
            return false;
        } else {
            return false;
        }
    }

    Name NameFormatter::get_name(const std::string &name_str) {
        Name ans;

        boost::char_separator<char> sep(&delim);
        typedef boost::tokenizer<boost::char_separator<char> > Tok;
        Tok tok(name_str, sep);
        for (Tok::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
            ans.unit_list.push_back(std::move(get_name_unit(*beg)));
        }
        return ans;
    }

    NameUnit NameFormatter::get_name_unit(const std::string &name_str) {
        NameUnit ans;

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
            // set base name
            ans.name = name_str.substr(0, vstart_idx);
            // set range
            ans.range = get_range(name_str.substr(vstart_idx + 1, name_str.size() - vstart_idx - 2));
        } else {
            // no vector start, assume this is scalar name
            ans.name = name_str;
        }

        return ans;
    }

    Range NameFormatter::get_range(const std::string &range_str) {

        boost::char_separator<char> sep(&vec_delim);
        typedef boost::tokenizer<boost::char_separator<char> > Tok;
        Tok tok(range_str, sep);
        uint32_t cnt = 0;
        int32_t r_start = 0;
        int32_t r_stop = 0;
        int32_t r_step = 0;
        for (Tok::iterator beg = tok.begin(); beg != tok.end(); ++beg, ++cnt) {
            switch (cnt) {
                case 0:
                    r_stop = r_start = std::stoi(*beg);
                    r_step = -1;
                    break;
                case 1:
                    r_stop = std::stoi(*beg);
                    r_step = (r_stop > r_start) ? 1 : -1;
                    break;
                case 2:
                    r_step = r_step * abs(std::stoi(*beg));
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

        int32_t n = (r_stop - r_start + r_step) / r_step;

        // return answer
        return {r_start, r_start + n * r_step, r_step};
    }

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const Transform &v) {
        return out << YAML::Flow
                   << YAML::BeginSeq << v.x << v.y << enumToStr(v.orient) << YAML::EndSeq;
    }

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const NameUnit &v) {
        return out << YAML::Flow
                   << YAML::BeginSeq << v.name << v.range.start << v.range.stop << v.range.step << YAML::EndSeq;
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const Name &v) {
        out << YAML::BeginSeq;
        for (const auto &name : v.unit_list) {
            out << name;
        }
        return out << YAML::EndSeq;
    }

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const value_t &v) {

        struct OutVisitor : public boost::static_visitor<> {

            explicit OutVisitor(YAML::Emitter *out_ptr)
                    : out_ptr(out_ptr) {}

            void operator()(const int32_t &i) const {
                (*out_ptr) << i;
            }

            void operator()(const double &d) const {
                (*out_ptr) << d;
            }

            void operator()(const std::string &s) const {
                (*out_ptr) << YAML::DoubleQuoted << s;
            }

            YAML::Emitter *out_ptr;

        };

        OutVisitor visitor(&out);
        boost::apply_visitor(visitor, v);
        return out;
    }

}

namespace std {

    // define hash function for NameUnit
    template<>
    struct hash<cbag::NameUnit> {
        size_t operator()(const cbag::NameUnit &v) const {

            size_t seed = 0;
            boost::hash_combine(seed, v.name);
            boost::hash_combine(seed, v.range.start);
            boost::hash_combine(seed, v.range.stop);
            boost::hash_combine(seed, v.range.step);

            return seed;
        }
    };

}