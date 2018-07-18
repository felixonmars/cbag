/** \file ast_adapted.h
 *  \brief This file adapts the AST structs as boost fusion tuples
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_AST_ADAPTED_H
#define CBAG_SPIRIT_AST_ADAPTED_H

#include <boost/fusion/adapted.hpp>

#include <cbag/spirit/ast.h>


// We need to tell fusion about our structs
// to make it a first-class fusion citizen. This has to
// be in global scope.

BOOST_FUSION_ADAPT_STRUCT(cbag::spirit::ast::range,
                          (uint32_t, start)(uint32_t, stop)(uint32_t, step)
)

BOOST_FUSION_ADAPT_STRUCT(cbag::spirit::ast::name_bit,
                          (std::string, base)(cbag::spirit::ast::name_bit::optint_t, index)
)

BOOST_FUSION_ADAPT_STRUCT(cbag::spirit::ast::name_unit,
                          (uint32_t, mult)(std::string, base)
                          (cbag::spirit::ast::range, idx_range)
)

BOOST_FUSION_ADAPT_STRUCT(cbag::spirit::ast::name,
                          (cbag::spirit::ast::name::namelist_t, unit_list)
)

#endif //CBAG_SPIRIT_AST_ADAPTED_H
