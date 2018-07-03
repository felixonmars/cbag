//
// Created by erichang on 6/27/18.
//

#ifndef CBAG_SCHEMATIC_OBJECTS_H
#define CBAG_SCHEMATIC_OBJECTS_H

#include <string>
#include <tuple>
#include <utility>

#include <cbag/common.h>

namespace cbag {

    struct CSchInstance {
        CSchInstance() = default;

        CSchInstance(std::string name, std::string lib, std::string cell, std::string view,
                     Transform xform, Range r)
                : inst_name(std::move(name)),
                  lib_name(std::move(lib)),
                  cell_name(std::move(cell)),
                  view_name(std::move(view)),
                  xform(xform), inst_range(r) {}

        inline uint32_t size() { return (inst_range.is_empty()) ? 1 : inst_range.size(); }

        std::string inst_name, lib_name, cell_name, view_name;
        Range inst_range;
        Transform xform;
        ParamMap params;
        std::map<std::string, std::vector<std::string>> term_map;
    };

    struct CSchMaster {
        CSchMaster() = default;

        std::list<CSchTerm> in_terms, out_terms, io_terms;
        std::list<CSchInstance> inst_list;
    };

// YAML stream out functions
    YAML::Emitter &operator<<(YAML::Emitter &out, const Range &v);

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchTerm &v);

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchInstance &v);

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchMaster &v);

}

#endif //CBAG_SCHEMATIC_OBJECTS_H
