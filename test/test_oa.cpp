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

/*
int write_oa() {
    try {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotAssignable"
        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, oacDataModelRevNumber); // NOLINT
#pragma clang diagnostic pop

        const oa::oaNativeNS ns;
        oa::oaString lib_def_path("cds.lib");
        oa::oaLibDefList::openLibs(lib_def_path);

        // open library
        oa::oaScalarName lib_name(ns, "AAAscratch");
        oa::oaScalarName cell_name(ns, "oa_mos_write");
        oa::oaScalarName view_name(ns, "layout");
        oa::oaLib *lib_ptr = oa::oaLib::find(lib_name);
        if (lib_ptr == nullptr) {
            throw std::invalid_argument("Cannot find library.");
        } else if (!lib_ptr->isValid()) {
            throw std::invalid_argument("Invalid library.");
        }

        oa::oaDesign *dsn_ptr = oa::oaDesign::open(lib_name, cell_name, view_name,
                                                   oa::oaViewType::get(oa::oacMaskLayout), 'w');


        oa::oaBlock *blk_ptr = oa::oaBlock::create(dsn_ptr);

        oa::oaScalarName mlib_name(ns, "tsmcN16");
        oa::oaScalarName mcell_name(ns, "nch_lvt_mac");
        oa::oaDesign *master_ptr = oa::oaDesign::open(mlib_name, mcell_name, view_name,
                                                      oa::oaViewType::get(oa::oacMaskLayout), 'r');


        oa::oaScalarName inst_name(ns, "X0");
        oa::oaTransform inst_tran(oa::oaPoint(0, 0));
        oa::oaParamArray parr;

        std::ifstream myfile("oa_mos_params.txt");
        std::string line;
        while (std::getline(myfile, line)) {
            std::istringstream iss(line);
            std::string name;
            std::string value;
            iss >> name;
            iss >> value;
            oa::oaParam par;
            par.setName(name.c_str());
            par.setStringVal(value.c_str());
            parr.append(par);
            std::cout << "adding par " << name << ": " << value << std::endl;
        }


        oa::oaScalarInst::create(blk_ptr, master_ptr, inst_name, inst_tran, &parr);

        dsn_ptr->save();
        dsn_ptr->close();
        lib_ptr->close();
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
*/

int main(int argc, char *argv[]) {
    return read_oa();
    // return write_oa(argc, argv);
}
