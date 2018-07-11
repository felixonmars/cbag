/** \file json.cpp
 *  \brief This file implements functions to convert various database objects to JSON.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */


#include <cbag/spirit/json.h>
#include <cbag/database/json.h>

using json = nlohmann::json;

namespace cbag {

    // Transform
    void to_json(json &j, const Transform &p) {
        j = json::array({p.x, p.y, p.orient._to_string()});
    }

    void from_json(const json &j, Transform &p) {
        p.x = j.at(0).get<coord_t>();
        p.y = j.at(1).get<coord_t>();
        p.orient = Orientation::_from_string(j.at(2).get<std::string>().c_str());
    }

    // Time
    void to_json(json &j, const Time &p) {
        j = json::array({"time", p.time_val});
    }

    void from_json(const json &j, Time &p) {
        p.time_val = j.at(1).get<time_t>();
    }

    // Binary
    void to_json(json &j, const Binary &p) {
        j = json::array({"bin", p.bin_val});
    }

    void from_json(const json &j, Binary &p) {
        p.bin_val = j.at(1).get<std::string>();
    }

    // SchInstance
    void to_json(json &j, const SchInstance &p) {
        j = json{{"lib_name",    p.lib_name},
                 {"cell_name",   p.cell_name},
                 {"view_name",   p.view_name},
                 {"xform",       p.xform},
                 {"connections", p.connections},
                 {"params",      p.params}};
    }

    void from_json(const json &j, SchInstance &p) {
        p.lib_name = j.at("lib_name").get<std::string>();
        p.cell_name = j.at("cell_name").get<std::string>();
        p.view_name = j.at("view_name").get<std::string>();
        p.xform = j.at("xform").get<Transform>();
        p.connections = j.at("connections").get<std::map<bsa::name_bit, std::vector<bsa::name_bit> > >();
        p.params = j.at("params").get<ParamMap>();
    }

    // SchMaster
    void to_json(json &j, const SchMaster &p) {
        j = json{{"symbols",   p.symbols},
                 {"in_pins",   p.in_pins},
                 {"out_pins",  p.out_pins},
                 {"io_pins",   p.io_pins},
                 {"instances", p.inst_map}};
    }

    void from_json(const json &j, SchMaster &p) {
        p.symbols = j.at("symbols").get<std::set<std::string>>();
        p.in_pins = j.at("in_pins").get<std::set<bsa::name>>();
        p.out_pins = j.at("out_pins").get<std::set<bsa::name>>();
        p.io_pins = j.at("io_pins").get<std::set<bsa::name>>();
        p.inst_map = j.at("instances").get<std::map<bsa::name_unit, SchInstance> >();
    }
}

// define JSON conversion functions for cbag::value_t in boost namespace, because
// cbag::value_t is a typedef of a boost data structure.
namespace boost {
    /** A visitor struct for writing boost:variant to a JSON file.
     */
    struct SetVisitor : public static_visitor<> {

        explicit SetVisitor(json *out_ptr) : out_ptr(out_ptr) {}

        void operator()(const int32_t &i) const {
            *out_ptr = json::array({"integer", i});
        }

        void operator()(const double &d) const {
            *out_ptr = json::array({"dbl", d});
        }

        void operator()(const std::string &s) const {
            *out_ptr = json::array({"str", s});
        }

        void operator()(const cbag::Time &t) const {
            *out_ptr = json{t};
        }

        void operator()(const cbag::Binary &v) const {
            *out_ptr = json{v};
        }

        json *out_ptr;

    };

    // value_t
    void to_json(json &j, const cbag::value_t &p) {
        SetVisitor visitor(&j);
        boost::apply_visitor(visitor, p);
    }

    void from_json(const json &j, cbag::value_t &p) {
        cbag::ParamType index = cbag::ParamType::_from_string(j.at(0).get<std::string>().c_str());
        switch (index) {
            case cbag::ParamType::integer :
                p = j.at(1).get<int32_t>();
                break;
            case cbag::ParamType::dbl :
                p = j.at(1).get<double>();
                break;
            case cbag::ParamType::str :
                p = j.at(1).get<std::string>();
                break;
            case cbag::ParamType::time :
                p = j.at(1).get<cbag::Time>();
                break;
            case cbag::ParamType::bin :
                p = j.at(1).get<cbag::Binary>();
                break;
        }
    }
}