/** \file parsers.h
 *  \brief This file defines the parsing method.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_PARSERS_H
#define CBAG_SPIRIT_PARSERS_H

#include <boost/spirit/home/x3.hpp>

#include <cbag/spirit/config.h>


namespace x3 = boost::spirit::x3;

namespace cbag {

    // define parse method template
    template<typename A, typename R>
    A parse(std::string const &source, R const &rule) {
        std::stringstream out;

        spirit::parser::iterator_type iter(source.begin());
        spirit::parser::iterator_type const end(source.end());

        // Our AST
        A ast;

        // Our error handler
        spirit::parser::error_handler_type error_handler(iter, end, out); // Our error handler
        const std::reference_wrapper<spirit::parser::error_handler_type> err_ref = std::ref(error_handler);
        // Our spirit
        auto const parser =
                // we pass our error handler to the spirit so we can access
                // it later on in our on_error and on_sucess handlers
                x3::with<spirit::parser::error_handler_tag>(err_ref)[rule];

        // Go forth and parse!
        if (x3::parse(iter, end, parser, ast) && iter == end) {
            return ast;
        } else {
            throw std::invalid_argument(out.str());
        }
    }
}

#endif //CBAG_SPIRIT_PARSERS_H
