//
// Created by erichang on 6/27/18.
//

#ifndef CXBASE_SCHEMATIC_OBJECTS_H
#define CXBASE_SCHEMATIC_OBJECTS_H

#include <string>
#include <tuple>
#include <vector>

#include <cxbase/common.h>

namespace cxbase {

    // Range data structure to represent bus terminal indices
    struct Range {
        Range() = default;

        Range(int32_t start, int32_t stop, int32_t step) : start(start), stop(stop), step(step) {}

        int32_t start, stop, step;
    };

    struct CSchTerm {
        CSchTerm() = default;

        explicit CSchTerm(std::string name) : name(std::move(name)), range_list() {}

        CSchTerm(std::string name, const std::vector<uint32_t> &idx_list);

        std::string name;
        std::vector<Range> range_list;
    };

    struct CSchInstance {
        CSchInstance() = default;

        std::string inst_name, lib_name, cell_name, view_name;
        Transform xform;
    };

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const Range &v) {
        return out << YAML::Flow << YAML::BeginSeq << v.start << v.stop << v.step << YAML::EndSeq;
    }

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const CSchTerm &v) {
        out << YAML::BeginMap
            << YAML::Key << "name"
            << YAML::Value << v.name
            << YAML::Key << "ranges"
            << YAML::Value << YAML::Flow << YAML::BeginSeq;

        for (Range item : v.range_list) {
            out << item;
        }
        out << YAML::EndSeq << YAML::EndMap;
        return out;
    }

}

#endif //CXBASE_OBJECTS_H
