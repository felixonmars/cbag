//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_AST_ADAPTED_H
#define CBAG_SPIRIT_AST_ADAPTED_H


#include <boost/fusion/include/adapt_struct.hpp>
#include <cbag/spirit/ast.h>

// We need to tell fusion about our structs
// to make it a first-class fusion citizen. This has to
// be in global scope.

BOOST_FUSION_ADAPT_STRUCT(cbag::spirit::ast::range,
                          start, stop, step
)

BOOST_FUSION_ADAPT_STRUCT(cbag::spirit::ast::name_unit,
                          base, index
)

#endif //CBAG_SPIRIT_AST_ADAPTED_H
