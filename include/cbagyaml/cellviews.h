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

#include <cbag/schematic/cellview_fwd.h>
#include <cbag/schematic/cellview_info.h>

#include <cbagyaml/fusion.h>

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::cellview, lib_name, cell_name, view_name, bbox, terminals,
                          shapes, instances, props, app_defs)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::cellview_info, cell_name, in_terms, out_terms, io_terms, props)

#endif // CBAG_YAML_CELLVIEWS_H
