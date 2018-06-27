//
// Created by Eric Chang on 6/11/18.
//

#ifndef CXBASE_DATABASE_H
#define CXBASE_DATABASE_H

#include <string>

namespace cxbase {
    class CMasterDB {
    public:
        CMasterDB(std::string lib_name, std::string prefix, std::string suffix);

        
    private:
        std::string lib_name, prefix, suffix;
    };
}

#endif //CXBASE_DATABASE_H
