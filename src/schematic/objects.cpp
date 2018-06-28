//
// Created by erichang on 6/27/18.
//

#include <cxbase/schematic/objects.h>


namespace cxbase {

    CSchTerm::CSchTerm(std::string name, const std::vector<uint32_t> &idx_list) :
            name(std::move(name)), range_list() {
        // assume idx_list sorted in increasing order.  Will generate range list in decreasing order
        auto riter = idx_list.rbegin();
        auto last = idx_list.rend();
        if (riter != last) {
            uint32_t start = *(riter++);
            uint32_t prev = start;
            int32_t step = 0;
            while (riter != last) {
                uint32_t stop = *(riter++);
                if (step == 0) {
                    step = (int32_t) (stop - prev);
                } else if (step != (int32_t) (stop - prev)) {
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
}


namespace YAML {
    // YAML encode/decode routine for Range
    template<>
    struct convert<cxbase::Range> {
        static Node encode(const cxbase::Range &rhs) {
            Node node;
            node.push_back(rhs.start);
            node.push_back(rhs.stop);
            node.push_back(rhs.step);
            return node;
        }

        static bool decode(const Node &node, cxbase::Range &rhs) {
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
    struct convert<cxbase::CSchTerm> {
        static Node encode(const cxbase::CSchTerm &rhs) {
            Node node, range_list_node;
            for (cxbase::Range item : rhs.range_list) {
                range_list_node.push_back(item);
            }
            node.push_back(rhs.name);
            node.push_back(range_list_node);
            return node;
        }

        static bool decode(const Node &node, cxbase::CSchTerm &rhs) {
            if (!node.IsSequence() || node.size() != 2) {
                return false;
            }

            for (YAML::const_iterator iter = node[1].begin(); iter != node[1].end(); ++iter) {
                rhs.range_list.push_back(iter->as<cxbase::Range>());
            }
            return true;
        }
    };
}

