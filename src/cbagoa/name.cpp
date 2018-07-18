/** \file name.h
 *  \brief This file defines routines for mapping name objects to strings in OpenAccess namespace.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#include <sstream>

#include <fmt/format.h>

#include <cbagoa/name.h>


namespace cbagoa {
    std::string to_string(const bsa::range &n) {
        if (n.step == 0) {
            return "";
        }

        if (n.start == n.stop) {
            return fmt::format("<{}>", n.start);
        } else {
            if (n.step == 1) {
                return fmt::format("<{}:{}>", n.start, n.stop);
            } else {
                return fmt::format("<{}:{}:{}>", n.start, n.stop, n.step);
            }
        }
    }

    std::string to_string(const bsa::name_bit &n) {
        return (n.index) ? fmt::format("{}<{}>", n.base, *n.index) : std::string(n.base);
    }

    std::string to_string(const bsa::name_unit &n) {
        if (n.mult > 1) {
            return fmt::format("<*{}>{}{}", n.mult, n.base, to_string(n.idx_range));
        } else {
            return n.base + to_string(n.idx_range);
        }
    }

    std::string to_string(const bsa::name &n) {
        size_t num = n.unit_list.size();
        if (num == 1) {
            return to_string(n.unit_list[0]);
        } else if (num > 1) {
            std::ostringstream builder;
            builder << to_string(n.unit_list[0]);
            for(size_t i = 1; i < num; i++) {
                builder << "," << to_string(n.unit_list[i]);
            }
            return builder.str();
        } else {
            return "";
        }
    }
}