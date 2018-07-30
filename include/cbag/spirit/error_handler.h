/** \file error_handler.h
 *  \brief This file defines the error handler for Spirit X3.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_ERROR_HANDLER_H
#define CBAG_SPIRIT_ERROR_HANDLER_H

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {
namespace parser {

// X3 Error result enum
using err_res = x3::error_handler_result;

// X3 Error Handler Utility
template <typename Iterator> using error_handler = x3::error_handler<Iterator>;

// tag used to get our error handler from the context
using error_handler_tag = x3::error_handler_tag;

struct error_handler_base {
    error_handler_base() = default;

    template <typename Iterator, typename Exception, typename Context>
    err_res on_error(Iterator &first, Iterator const &last, Exception const &x,
                     Context const &context);
};

template <typename Iterator, typename Exception, typename Context>
inline err_res
error_handler_base::on_error(Iterator &first, Iterator const &last,
                             Exception const &x, Context const &context) {
    std::string message =
        "Error while parsing index: " + std::to_string(x.where() - first);
    auto &error_handler = x3::get<error_handler_tag>(context).get();
    error_handler(x.where(), message);
    return x3::error_handler_result::fail;
}
} // namespace parser
} // namespace spirit
} // namespace cbag

#endif // CBAG_SPIRIT_ERROR_HANDLER_H
