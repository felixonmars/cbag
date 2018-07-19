#include <iostream>

#include <cbag/yaml_fusion.h>

#include <cbag/spirit/ast_adapted.h>
#include <cbag/spirit/parsers.h>
#include <cbag/spirit/name.h>


namespace bsp = cbag::spirit;

int main() {
    std::cout << "Give me a name." << std::endl;
    std::cout << "Type [q or Q] to quit" << std::endl << std::endl;

    std::string str;
    while (getline(std::cin, str)) {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;


        try {
            std::ostringstream ofs;
            auto name_obj = cbag::parse<bsp::ast::name,
                    bsp::parser::name_type>(str.c_str(), str.size(), bsp::name());

            std::cout << "-------------------------\n";
            std::cout << "Success.  Output: \n";
            std::cout << yaml::serialization::to_yaml(name_obj) << std::endl;
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