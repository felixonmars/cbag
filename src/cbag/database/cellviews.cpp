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

    void SchCellView::clear_params() {
        props.clear();
    }

    void SchCellView::set_int_param(const char *name, int value) {
        props.insert_or_assign(name, value);
    }


    void SchCellView::set_double_param(const char *name, double value) {
        props.insert_or_assign(name, value);
    }

    void SchCellView::set_bool_param(const char *name, bool value) {
        props.insert_or_assign(name, value);
    }

    void SchCellView::set_string_param(const char *name, const char *value) {
        props.insert_or_assign(name, value);
    }
}
