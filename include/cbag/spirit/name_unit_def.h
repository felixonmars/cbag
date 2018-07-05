//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_NAME_UNIT_DEF_H
#define CBAG_SPIRIT_NAME_UNIT_DEF_H

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/ast_adapted.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/name_unit.h>
#include <cbag/spirit/range_def.h>

namespace cbag {
    namespace spirit {
        namespace parser {
            namespace x3 = boost::spirit::x3;

            name_unit_type const name_unit = "name_unit";

            auto const name_string = +(x3::ascii::print - x3::ascii::char_("<>:* "));

            auto const name_unit_def = name_unit_type{}
                = -('<' > x3::uint32[check_zero] > "*>") > name_string >> -(range);

            BOOST_SPIRIT_DEFINE(name_unit);

            struct name_unit_class : x3::annotate_on_success, error_handler_base {
            };
        }

    }
}
#endif //CBAG_SPIRIT_NAME_UNIT_DEF_H
