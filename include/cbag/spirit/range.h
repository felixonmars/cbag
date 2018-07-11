/** \file range.h
 *  \brief This file declares the parsing rule for range.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_RANGE_H
#define CBAG_SPIRIT_RANGE_H

#include <boost/spirit/home/x3.hpp>

#include <cbag/spirit/ast.h>


namespace x3 = boost::spirit::x3;

namespace cbag {
    namespace spirit {
        namespace parser {

            struct range_class;

            using range_type = x3::rule<range_class, ast::range, true>;

            BOOST_SPIRIT_DECLARE(range_type);
        }

        parser::range_type const &range();
    }
}

#endif //CBAG_SPIRIT_RANGE_H
