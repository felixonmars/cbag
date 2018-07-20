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

    void SchCellView::rename_pin(const char *old_name, const char *new_name) {
        std::string key(old_name);
        auto nh = in_terms.extract(key);
        if (nh.empty()) {
            nh = out_terms.extract(key);
            if (nh.empty()) {
                nh = io_terms.extract(key);
                if (nh.empty()) {
                    throw std::invalid_argument("Cannot find terminal: " + key);
                }
                nh.key() = std::string(new_name);
                io_terms.insert(std::move(nh));
            } else {
                nh.key() = std::string(new_name);
                out_terms.insert(std::move(nh));
            }
        } else {
            nh.key() = std::string(new_name);
            in_terms.insert(std::move(nh));
        }
    }

    void SchCellView::rename_instance(const char *old_name, const char *new_name) {
        std::string key(old_name);
        auto nh = instances.extract(key);
        if (nh.empty()) {
            throw std::invalid_argument("Cannot find instance: " + key);
        } else {
            nh.key() = std::string(new_name);
            instances.insert(std::move(nh));
        }
    }
}
