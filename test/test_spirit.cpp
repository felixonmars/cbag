#include <iostream>
#include <fstream>

#include <boost/archive/xml_oarchive.hpp>

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
            boost::archive::xml_oarchive xml_out(ofs);
            auto name_obj = cbag::parse<bsp::ast::name,
                    bsp::parser::name_type>(str.c_str(), str.size(), bsp::name());
            xml_out << BOOST_SERIALIZATION_NVP(name_obj);

            std::cout << "-------------------------\n";
            std::cout << "Success.  Output: \n";
            std::cout << ofs.str() << std::endl;
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