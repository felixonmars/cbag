//
// Created by erichang on 6/27/18.
//

#include <cbag/spirit/yaml.h>
#include <cbag/schematic/objects.h>


namespace cbag {

    CSchInstance::CSchInstance(bsa::name_unit name, std::string lib, std::string cell,
                               std::string view, Transform xform)
            : inst_name(std::move(name)), lib_name(std::move(lib)), cell_name(std::move(cell)),
              view_name(std::move(view)), xform(xform) {
        if (inst_name.mult != 1) {
            throw std::invalid_argument("Invalid instance name.");
        }
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchInstance &v) {
        out << YAML::BeginMap
            << YAML::Key << "inst_name" << YAML::Value << v.inst_name
            << YAML::Key << "lib_name" << YAML::Value << v.lib_name
            << YAML::Key << "cell_name" << YAML::Value << v.cell_name
            << YAML::Key << "view_name" << YAML::Value << v.view_name
            << YAML::Key << "xform" << YAML::Value << v.xform
            << YAML::Key << "in_pins" << YAML::Value << v.in_pins
            << YAML::Key << "out_pins" << YAML::Value << v.out_pins
            << YAML::Key << "io_pins" << YAML::Value << v.io_pins
            << YAML::Key << "connections" << YAML::Value << v.connections
            << YAML::Key << "params" << YAML::Value << YAML::BeginMap;

        for (const auto &element : v.params) {
            out << YAML::Key << element.first << YAML::Value << element.second;
        }
        out << YAML::EndMap;

        out << YAML::EndMap;
        return out;
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchMaster &v) {
        return out << YAML::BeginMap
                   << YAML::Key << "in_pins" << YAML::Value << v.in_pins
                   << YAML::Key << "out_pins" << YAML::Value << v.out_pins
                   << YAML::Key << "io_pins" << YAML::Value << v.io_pins
                   << YAML::Key << "instances" << YAML::Value << v.inst_list
                   << YAML::EndMap;
    }
}
