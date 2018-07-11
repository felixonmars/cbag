/** \file json.cpp
 *  \brief This file implements functions to read/write various struct objects to JSON.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */


#include <cbag/spirit/json.h>

using json = nlohmann::json;

namespace cbag {
    namespace spirit {
        namespace ast {

            // range
            void to_json(json &j, const range &p) {
                j = json::array({p.start, p.stop, p.step});
            }

            void from_json(const json &j, range &p) {
                p.start = j.at(0).get<uint32_t>();
                p.stop = j.at(1).get<uint32_t>();
                p.step = j.at(2).get<uint32_t>();
            }

            // name_bit
            void to_json(json &j, const name_bit &p) {
                if (p.index) {
                    j = json::array({p.base, *p.index});
                } else {
                    j = json::array({p.base});
                }
            }

            void from_json(const json &j, name_bit &p) {
                p.base = j.at(0).get<std::string>();
                if (j.size() > 1) {
                    p.index = j.at(1).get<uint32_t>();
                }
            }

            // name_unit
            void to_json(json &j, const name_unit &p) {
                j = json::array({p.base, p.idx_range, p.mult});
            }

            void from_json(const json &j, name_unit &p) {
                p.base = j.at(0).get<std::string>();
                p.idx_range = j.at(1).get<range>();
                p.mult = j.at(2).get<uint32_t>();
            }

            // name
            void to_json(json &j, const name &p) {
                j = p.unit_list;
            }

            void from_json(const json &j, name &p) {
                p.unit_list = j.get<std::vector<name_unit> >();
            }
        }
    }
}