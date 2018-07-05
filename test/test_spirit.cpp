#include <cbag/spirit/ast.h>
#include <cbag/spirit/ast_adapted.h>
#include <cbag/spirit/range.h>

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main() {
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tAn range spirit for Spirit...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";

    std::cout << "Give me an range." << std::endl;
    std::cout << "Type [q or Q] to quit" << std::endl << std::endl;

    using iterator_type = std::string::const_iterator;

    std::string str;
    while (getline(std::cin, str)) {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        cbag::spirit::ast::range rang;
        iterator_type iter = str.begin();
        iterator_type const end = str.end();
        bool r = parse(iter, end, cbag::spirit::range(), rang);

        if (r && iter == end) {
            std::cout << boost::fusion::tuple_open('[');
            std::cout << boost::fusion::tuple_close(']');
            std::cout << boost::fusion::tuple_delimiter(", ");

            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "got: " << rang << std::endl;
            std::cout << "\n-------------------------\n";
        } else {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
