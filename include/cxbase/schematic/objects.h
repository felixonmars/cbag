//
// Created by erichang on 6/27/18.
//

#ifndef CXBASE_SCHEMATIC_OBJECTS_H
#define CXBASE_SCHEMATIC_OBJECTS_H

#include <string>
#include <tuple>
#include <vector>

#include <yaml-cpp/yaml.h>

namespace cxbase {

    struct Range {
        Range() = default;

        Range(uint32_t start, uint32_t stop, int32_t step) : start(start), stop(stop), step(step) {}

        uint32_t start, stop;
        int32_t step;
    };

    struct CSchTerm {
        CSchTerm() = default;

        explicit CSchTerm(std::string name) : name(std::move(name)), range_list() {}

        CSchTerm(std::string name, const std::vector<uint32_t> &idx_list);

        std::string name;
        std::vector<Range> range_list;
    };

    YAML::Emitter &operator<<(YAML::Emitter &out, const Range &v) {
        out << YAML::BeginSeq << v.start << v.stop << v.step << YAML::EndSeq;
        return out;
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchTerm &v) {
        out << YAML::BeginMap
            << YAML::Key << "name"
            << YAML::Value << v.name
            << YAML::Key << "ranges"
            << YAML::Value << YAML::BeginSeq;

        for (Range item : v.range_list) {
            out << item;
        }
        out << YAML::EndSeq << YAML::EndMap;
        return out;
    }

}


#endif //CXBASE_OBJECTS_H
