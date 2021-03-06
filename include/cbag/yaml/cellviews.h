/** \file yaml_cellviews.h
 *  \brief This file declares YAML serialization methods for cellviews.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */
#ifndef CBAG_YAML_CELLVIEWS_H
#define CBAG_YAML_CELLVIEWS_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <yaml-cpp/yaml.h>

#include "yaml-cpp/unique_ptr.h"
#include "yaml-cpp/unordered_map.h"

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/yaml/box_t.h>
#include <cbag/yaml/datatypes.h>
#include <cbag/yaml/enum.h>
#include <cbag/yaml/figures.h>
#include <cbag/yaml/fusion.h>
#include <cbag/yaml/shapes.h>
#include <cbag/yaml/sorted_map.h>
#include <cbag/yaml/transformation.h>

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::cellview, lib_name, cell_name, view_name, bbox, terminals,
                          shapes, instances, props, app_defs)

namespace YAML {

template <> struct convert<cbag::sch::cellview_info> {
    static Node encode(const cbag::sch::cellview_info &rhs);

    static bool decode(const Node &node, cbag::sch::cellview_info &rhs);
};

} // namespace YAML

#endif // CBAG_YAML_CELLVIEWS_H
