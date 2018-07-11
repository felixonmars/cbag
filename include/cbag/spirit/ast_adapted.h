/** \file ast_adapted.h
 *  \brief This file adapts the AST structs as boost fusion tuples
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

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
                          mult, base, idx_range
)

BOOST_FUSION_ADAPT_STRUCT(cbag::spirit::ast::name,
                          unit_list
)

#endif //CBAG_SPIRIT_AST_ADAPTED_H
