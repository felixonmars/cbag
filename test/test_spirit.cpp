#include <iostream>

#include <yaml-cpp/yaml.h>

#include <cbag/spirit/parsers.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/yaml.h>

int main() {
    std::cout << "Give me a name." << std::endl;
    std::cout << "Type [q or Q] to quit" << std::endl << std::endl;

    std::string str;
    while (getline(std::cin, str)) {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;


        try {
            YAML::Emitter yout;
            yout << cbag::parse(str);
            std::cout << "-------------------------\n";
            std::cout << "Success.  Output: \n";
            std::cout << yout.c_str() << std::endl;
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