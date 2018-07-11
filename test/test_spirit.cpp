#include <iostream>

#include <nlohmann/json.hpp>

#include <cbag/spirit/parsers.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/json.h>


namespace bsp = cbag::spirit;

int main() {
    std::cout << "Give me a name." << std::endl;
    std::cout << "Type [q or Q] to quit" << std::endl << std::endl;

    std::string str;
    while (getline(std::cin, str)) {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;


        try {
            bsp::ast::name name_obj = cbag::parse<bsp::ast::name, bsp::parser::name_type>(str, bsp::name());
            nlohmann::json obj = name_obj;

            std::cout << "-------------------------\n";
            std::cout << "Success.  Output: \n";
            std::cout << obj.dump(2) << std::endl;
            std::cout << "-------------------------\n";
        } catch (std::invalid_argument &ex) {
            std::cout << "-------------------------\n";
            std::cout << "Failed.  Error message: \n";
            std::cout << ex.what() << std::endl;
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}