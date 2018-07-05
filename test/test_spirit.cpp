#include <iostream>
#include <boost/fusion/include/io.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/range.h>
#include <cbag/spirit/error_handler.h>
#include <cbag/spirit/config.h>


std::string parse(std::string const &source) {
    std::stringstream out;

    using cbag::spirit::parser::iterator_type;
    iterator_type iter(source.begin());
    iterator_type const end(source.end());

    // Our AST
    cbag::spirit::ast::range ast;

    // Our error handler
    using boost::spirit::x3::with;
    using cbag::spirit::parser::error_handler_type;
    using cbag::spirit::parser::error_handler_tag;
    error_handler_type error_handler(iter, end, out); // Our error handler
    const std::reference_wrapper<error_handler_type> err_ref = std::ref(error_handler);
    // Our parser
    auto const parser =
            // we pass our error handler to the parser so we can access
            // it later on in our on_error and on_sucess handlers
            with<error_handler_tag>(err_ref)
            [
                    cbag::spirit::range()
            ];

    // Go forth and parse!
    bool success = parse(iter, end, parser, ast);

    if (success) {
        if (iter != end) {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
            error_handler(iter, "Error! Expecting end of input here: ");
        } else {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "got: " << ast.start << ", " << ast.stop << ", " << ast.step << std::endl;
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
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tAn range spirit for Spirit...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";

    std::cout << "Give me an range." << std::endl;
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
