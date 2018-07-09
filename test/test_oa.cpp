#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <oa/oaDesignDB.h>

#include <cbagoa/database.h>


int read_oa() {
    try {

        std::string lib_file("cds.lib");
        std::string library("AAAAAASCRATCH");
        std::string lib_path(".");
        std::string tech_lib("tsmcN16");
        std::string cell_name("inv");
        std::string view_name("schematic");

        cbagoa::Library lib;

        lib.open_lib(lib_file, library, lib_path, tech_lib);

        cbag::CSchMaster sch_master = lib.parse_sch(cell_name, view_name);

        YAML::Emitter out_yaml;
        out_yaml << sch_master;

        std::cout << out_yaml.c_str() << std::endl;

        lib.close();
    } catch (oa::oaCompatibilityError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA Compatibility Error: " + msg_std);
    } catch (oa::oaDMError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA DM Error: " + msg_std);
    } catch (oa::oaError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA Error: " + msg_std);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    return read_oa();
}
