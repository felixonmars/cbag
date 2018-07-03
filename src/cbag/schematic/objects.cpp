//
// Created by erichang on 6/27/18.
//

#include <cbag/schematic/objects.h>

namespace cbag {

    CSchTerm::CSchTerm(std::string name, const std::list<uint32_t> &idx_list) :
            name(std::move(name)), range_list() {
        // assume idx_list sorted in increasing order.  Will generate range list in decreasing order
        auto riter = idx_list.rbegin();
        auto last = idx_list.rend();
        if (riter != last) {
            int32_t start = *(riter++);
            int32_t prev = start;
            int32_t step = 0;
            while (riter != last) {
                int32_t stop = *(riter++);
                if (step == 0) {
                    step = stop - prev;
                } else if (step != stop - prev) {
                    // step size changed
                    range_list.emplace_back(start, prev + step, step);
                    start = stop;
                    step = 0;
                }
                prev = stop;
            }
            if (step == 0) {
                // single value left over
                range_list.emplace_back(start, start - 1, -1);
            } else {
                // multiple values left over
                range_list.emplace_back(start, prev + step, step);
            }
        }
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const Range &v) {
        return out << YAML::Flow << YAML::BeginSeq << v.start << v.stop << v.step << YAML::EndSeq;
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchTerm &v) {
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

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchInstance &v) {
        // record basic info
        out << YAML::BeginMap
            << YAML::Key << "inst_name"
            << YAML::Value << v.inst_name
            << YAML::Key << "lib_name"
            << YAML::Value << v.lib_name
            << YAML::Key << "cell_name"
            << YAML::Value << v.cell_name
            << YAML::Key << "view_name"
            << YAML::Value << v.view_name
            << YAML::Key << "xform"
            << YAML::Value << v.xform;

        // record parameters
        out << YAML::Key << "params"
            << YAML::Value << YAML::BeginMap;
        for (const auto &element : v.params) {
            out << YAML::Key << element.first << YAML::Value << element.second;
        }
        out << YAML::EndMap;

        // record terminals

        // wrap up and return
        out << YAML::EndMap;
        return out;
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchMaster &v) {
        return out << YAML::BeginMap
                   << YAML::Key << "inputs"
                   << YAML::Value << YAML::BeginSeq << v.in_terms << YAML::EndSeq
                   << YAML::Key << "outputs"
                   << YAML::Value << YAML::BeginSeq << v.out_terms << YAML::EndSeq
                   << YAML::Key << "inouts"
                   << YAML::Value << YAML::BeginSeq << v.io_terms << YAML::EndSeq
                   << YAML::Key << "instances"
                   << YAML::Value << YAML::BeginSeq << v.inst_list << YAML::EndSeq
                   << YAML::EndMap;
    }
}

namespace YAML {
// YAML encode/decode routine for Range
    template<>
    struct convert<cbag::Range> {
        static Node encode(const cbag::Range &rhs) {
            Node node;
            node.push_back(rhs.start);
            node.push_back(rhs.stop);
            node.push_back(rhs.step);
            return node;
        }

        static bool decode(const Node &node, cbag::Range &rhs) {
            if (!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.start = node[0].as<uint32_t>();
            rhs.stop = node[1].as<uint32_t>();
            rhs.step = node[2].as<int32_t>();
            return true;
        }
    };

// YAML encode/decode routine for CSchTerm
    template<>
    struct convert<cbag::CSchTerm> {
        static Node encode(const cbag::CSchTerm &rhs) {
            Node node, range_list_node;
            for (cbag::Range item : rhs.range_list) {
                range_list_node.push_back(item);
            }
            node.push_back(rhs.name);
            node.push_back(range_list_node);
            return node;
        }

        static bool decode(const Node &node, cbag::CSchTerm &rhs) {
            if (!node.IsSequence() || node.size() != 2) {
                return false;
            }

            for (YAML::const_iterator iter = node[1].begin(); iter != node[1].end(); ++iter) {
                rhs.range_list.push_back(iter->as<cbag::Range>());
            }
            return true;
        }
    };
}

