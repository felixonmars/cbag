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

template <typename A, typename R> void parse(const std::string &source, R const &rule, A &ast) {
    if (source.empty())
        throw std::invalid_argument("Cannot parse empty string.");

    std::stringstream out;

    auto iter_start = source.begin();
    auto const iter_end = source.end();

    // Our error handler
    spirit::parser::error_handler_type error_handler(iter_start, iter_end, out);
    const std::reference_wrapper<spirit::parser::error_handler_type> err_ref =
        std::ref(error_handler);
    // Our spirit
    auto const parser =
        // we pass our error handler to the spirit so we can access
        // it later on in our on_error and on_sucess handlers
        x3::with<spirit::parser::error_handler_tag>(err_ref)[rule];

    // Go forth and parse!
    if (!x3::parse(iter_start, iter_end, parser, ast) || iter_start != iter_end) {
        throw std::invalid_argument(out.str());
    }
}

} // namespace cbag

#endif // CBAG_SPIRIT_PARSERS_H
