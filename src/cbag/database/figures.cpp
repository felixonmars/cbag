/** \file figures.cpp
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <cbag/database/figures.h>

namespace cbag {

    void Instance::clear_params() {
        params.clear();
    }

    void Instance::set_int_param(const char *name, int value) {
        params.insert_or_assign(name, value);
    }


    void Instance::set_double_param(const char *name, double value) {
        params.insert_or_assign(name, value);
    }

    void Instance::set_bool_param(const char *name, bool value) {
        params.insert_or_assign(name, value);
    }

    void Instance::set_string_param(const char *name, const char *value) {
        params.insert_or_assign(name, value);
    }
}