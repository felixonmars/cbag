/** \file json.h
 *  \brief This file defines functions to read/write various struct objects to JSON.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_JSON_H
#define CBAG_SPIRIT_JSON_H

#include <nlohmann/json.hpp>

#include <cbag/spirit/ast.h>

using json = nlohmann::json;

namespace cbag {
    namespace spirit {
        namespace ast {

            // range
            void to_json(json &j, const range &p);

            void from_json(const json &j, range &p);

            // name_bit
            void to_json(json &j, const name_bit &p);

            void from_json(const json &j, name_bit &p);

            // name_unit
            void to_json(json &j, const name_unit &p);

            void from_json(const json &j, name_unit &p);

            // name
            void to_json(json &j, const name &p);

            void from_json(const json &j, name &p);
        }
    }
}

#endif //CBAG_SPIRIT_JSON_H
