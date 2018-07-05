#include <iostream>
#include <boost/fusion/include/io.hpp>

#include <yaml-cpp/yaml.h>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/config.h>
#include <cbag/spirit/yaml.h>


namespace x3 = boost::spirit::x3;
namespace bsp = cbag::spirit;


std::string parse(std::string const &source) {
    std::stringstream out;

    bsp::parser::iterator_type iter(source.begin());
    bsp::parser::iterator_type const end(source.end());

    // Our AST
    bsp::ast::name ast;

    // Our error handler
    bsp::parser::error_handler_type error_handler(iter, end, out); // Our error handler
    const std::reference_wrapper<bsp::parser::error_handler_type> err_ref = std::ref(error_handler);
    // Our spirit
    auto const parser =
            // we pass our error handler to the spirit so we can access
            // it later on in our on_error and on_sucess handlers
            x3::with<bsp::parser::error_handler_tag>(err_ref)
            [
                    bsp::name()
            ];

    // Go forth and parse!
    bool success = x3::parse(iter, end, parser, ast);

    if (success) {
        if (iter != end) {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
            error_handler(iter, "Error! Expecting end of input here: ");
        } else {
            YAML::Emitter yout;

            yout << ast;

            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "got: " << yout.c_str() << std::endl;
            std::cout << "-------------------------\n";
        }
    } else {
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "-------------------------\n";
    }

    return out.str();
};

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main() {
    std::cout << "Give me a name." << std::endl;
    std::cout << "Type [q or Q] to quit" << std::endl << std::endl;

    std::string str;
    while (getline(std::cin, str)) {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        std::string out = parse(str);
        std::cout << "-------------------------\n";
        std::cout << "Output: \n";
        std::cout << out << std::endl;
        std::cout << "-------------------------\n";
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
