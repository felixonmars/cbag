//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_ERROR_HANDLER_H
#define CBAG_SPIRIT_ERROR_HANDLER_H

#include <map>

#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>


namespace cbag {
    namespace spirit {
        namespace parser {
            namespace x3 = boost::spirit::x3;

            // X3 Error Handler Utility
            template<typename Iterator>
            using error_handler = x3::error_handler<Iterator>;

            // tag used to get our error handler from the context
            using error_handler_tag = x3::error_handler_tag;

            struct error_handler_base {
                error_handler_base() = default;

                template<typename Iterator, typename Exception, typename Context>
                x3::error_handler_result on_error(
                        Iterator &first, Iterator const &last, Exception const &x, Context const &context);
            };

            template<typename Iterator, typename Exception, typename Context>
            inline x3::error_handler_result
            error_handler_base::on_error(
                    Iterator &first, Iterator const &last, Exception const &x, Context const &context) {
                std::string message = "Error while parsing index: " + std::to_string(x.where() - first);
                auto &error_handler = x3::get<error_handler_tag>(context).get();
                error_handler(x.where(), message);
                return x3::error_handler_result::fail;
            }
        }
    }
}

#endif //CBAG_SPIRIT_ERROR_HANDLER_H
