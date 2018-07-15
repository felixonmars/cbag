/** \file name.h
 *  \brief This file defines routines for mapping name objects to strings in OpenAccess namespace.
 *
 *  \author Eric Chang
 *  \date   2018/07/14
 */

#ifndef CBAGOA_NAME_H
#define CBAGOA_NAME_H

#include <string>

#include <cbag/spirit/ast.h>


namespace bsa = cbag::spirit::ast;

namespace cbagoa {
    std::string to_string(const bsa::range &n);

    std::string to_string(const bsa::name_bit &n);

    std::string to_string(const bsa::name_unit &n);

    std::string to_string(const bsa::name &n);
}

#endif //CBAGOA_NAME_H
