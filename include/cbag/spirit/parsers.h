//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_PARSERS_H
#define CBAG_SPIRIT_PARSERS_H

#include <cbag/spirit/ast.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/config.h>

namespace cbag {

    namespace x3 = boost::spirit::x3;
    namespace bsp = cbag::spirit;

    template <typename A, typename R>
    A parse(std::string const &source, const R &rule) {
        std::stringstream out;

        bsp::parser::iterator_type iter(source.begin());
        bsp::parser::iterator_type const end(source.end());

        // Our AST
        A ast;

        // Our error handler
        bsp::parser::error_handler_type error_handler(iter, end, out); // Our error handler
        const std::reference_wrapper<bsp::parser::error_handler_type> err_ref = std::ref(error_handler);
        // Our spirit
        auto const parser =
                // we pass our error handler to the spirit so we can access
                // it later on in our on_error and on_sucess handlers
                x3::with<bsp::parser::error_handler_tag>(err_ref)
                [
                        rule
                ];

        // Go forth and parse!
        if (!x3::parse(iter, end, parser, ast)) {
            throw std::invalid_argument(out.str());
        }
        return ast;
    };

}

#endif //CBAG_SPIRIT_PARSERS_H
