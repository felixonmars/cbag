//
// Created by erichang on 7/5/18.
//

#include <iostream>

#include <cbag/spirit/parsers.h>

#include <cbag/spirit/name.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/config.h>

namespace x3 = boost::spirit::x3;

namespace cbag {
    spirit::ast::name parse(std::string const &source) {
        std::stringstream out;

        spirit::parser::iterator_type iter(source.begin());
        spirit::parser::iterator_type const end(source.end());

        // Our AST
        spirit::ast::name ast;

        // Our error handler
        spirit::parser::error_handler_type error_handler(iter, end, out); // Our error handler
        const std::reference_wrapper<spirit::parser::error_handler_type> err_ref = std::ref(error_handler);
        // Our spirit
        auto const parser =
                // we pass our error handler to the spirit so we can access
                // it later on in our on_error and on_sucess handlers
                x3::with<spirit::parser::error_handler_tag>(err_ref)
                [
                        spirit::name()
                ];

        // Go forth and parse!
        if (x3::parse(iter, end, parser, ast)) {
            if (iter != end) {
                error_handler(iter, "Error! Unexpected end of input here: ");
                throw std::invalid_argument(out.str());
            }
            return ast;
        } else {
            throw std::invalid_argument(out.str());
        }
    }

}