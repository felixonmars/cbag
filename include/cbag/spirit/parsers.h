//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_PARSERS_H
#define CBAG_SPIRIT_PARSERS_H

#include <cbag/spirit/ast.h>

namespace cbag {
    spirit::ast::name parse(std::string const &source);
}

#endif //CBAG_SPIRIT_PARSERS_H
