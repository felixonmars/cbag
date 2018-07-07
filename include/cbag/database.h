//
// Created by Eric Chang on 6/11/18.
//

#ifndef CBAG_DATABASE_H
#define CBAG_DATABASE_H

#include <string>


namespace cbag {
    class CMasterDB {
    public:
        CMasterDB(std::string lib_name, std::string prefix, std::string suffix);

    private:
        std::string lib_name, prefix, suffix;
    };
}

#endif //CBAG_DATABASE_H
