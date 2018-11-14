/** \file yaml_figures.h
 *  \brief This file declares YAML serialization methods for figures.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_FIGURES_H
#define CBAG_YAML_FIGURES_H

#include <boost/fusion/include/adapt_struct.hpp>

#include <yaml-cpp/yaml.h>

#include <cbag/schematic/instance.h>
#include <cbag/schematic/pin_figure.h>
#include <cbag/schematic/pin_object.h>

#include <cbagyaml/fusion.h>
#include <cbagyaml/sorted_map.h>

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::instance, lib_name, cell_name, view_name, xform, bbox,
                          connections, params, is_primitive)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::pin_object, inst, attr)

BOOST_FUSION_ADAPT_STRUCT(cbag::sch::pin_figure, obj, stype, ttype)

namespace YAML {
template <> struct convert<cbag::sch::pin_fig_t> {
    static Node encode(const cbag::sch::pin_fig_t &rhs);

    static bool decode(const Node &node, cbag::sch::pin_fig_t &rhs);
};
} // namespace YAML

#endif // CBAG_YAML_FIGURES_H
