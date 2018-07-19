/** \file cellviews.cpp
 *  \brief This file defines classes representing various cellviews
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#include <easylogging++.h>

#include <cbag/database/yaml_cellviews.h>


INITIALIZE_EASYLOGGINGPP // NOLINT

namespace cbag {
    SchCellView::SchCellView(const char *yaml_fname) {
        YAML::Node n = YAML::Load(yaml_fname);
        LOG(INFO) << "Loading cv";
        (*this) = n.as<SchCellView>();
    }
}
