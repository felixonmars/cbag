//
// Created by erichang on 6/27/18.
//

#include <cbag/spirit/yaml.h>
#include <cbag/schematic/objects.h>


namespace cbag {

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchInstance &v) {
        out << YAML::BeginMap
            << YAML::Key << "lib_name" << YAML::Value << v.lib_name
            << YAML::Key << "cell_name" << YAML::Value << v.cell_name
            << YAML::Key << "view_name" << YAML::Value << v.view_name
            << YAML::Key << "xform" << YAML::Value << v.xform
            << YAML::Key << "connections" << YAML::Value << v.connections
            << YAML::Key << "params" << YAML::Value << v.params;

        out << YAML::EndMap;
        return out;
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchMaster &v) {
        return out << YAML::BeginMap
                   << YAML::Key << "in_pins" << YAML::Value << v.in_pins
                   << YAML::Key << "out_pins" << YAML::Value << v.out_pins
                   << YAML::Key << "io_pins" << YAML::Value << v.io_pins
                   << YAML::Key << "instances" << YAML::Value << v.inst_map
                   << YAML::EndMap;
    }
}
