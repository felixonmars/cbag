/** \file json.h
 *  \brief This file defines functions to convert various database objects to JSON.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */


#ifndef CBAG_DATABASE_JSON_H
#define CBAG_DATABASE_JSON_H

#include <nlohmann/json.hpp>

#include <cbag/database/datatypes.h>
#include <cbag/database/shapes.h>
#include <cbag/database/cellviews.h>

using json = nlohmann::json;

namespace cbag {

    // Transform
    void to_json(json &j, const Transform &p);

    void from_json(const json &j, Transform &p);

    // Time
    void to_json(json &j, const Time &p);

    void from_json(const json &j, Time &p);

    // Binary
    void to_json(json &j, const Binary &p);

    void from_json(const json &j, Binary &p);

    // SchInstance
    void to_json(json &j, const SchInstance &p);

    void from_json(const json &j, SchInstance &p);

    // SchMaster
    void to_json(json &j, const SchMaster &p);

    void from_json(const json &j, SchMaster &p);

}


// define JSON conversion functions for cbag::value_t in boost namespace, because
// cbag::value_t is a typedef of a boost data structure.
namespace boost {

    // value_t
    void to_json(json &j, const cbag::value_t &p);

    void from_json(const json &j, cbag::value_t &p);
}

#endif //CBAG_DATABASE_JSON_H
