/** \file cellviews.cpp
 *  \brief This file defines classes representing various cellviews
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#include <cbag/database/yaml_cellviews.h>


namespace cbag {
    SchCellView::SchCellView(const std::string &yaml_fname) {
        YAML::Node n = YAML::LoadFile(yaml_fname);
        (*this) = n.as<SchCellView>();
    }
}
