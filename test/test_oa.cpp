#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

#include <boost/archive/xml_oarchive.hpp>

#include <oa/oaDesignDB.h>

#include <cbagoa/database.h>


int read_oa() {
    try {

        std::string lib_file("cds.lib");
        std::string lib_path(".");
        std::string tech_lib("cds_ff_mpt");
        std::string lib_name("schtest");
        std::string cell_name("inv");
        std::string view_name("schematic");
        std::string sym_view_name("symbol");

        cbagoa::OADatabase db(lib_file);

        cbag::SchCellView sch_master = db.read_sch_cellview(lib_name, cell_name, view_name);

        cbag::SchCellView sym_master = db.read_sch_cellview(lib_name, cell_name, sym_view_name);

        std::ofstream outfile;
        outfile.open("inv_sch.xml", std::ios_base::out);
        boost::archive::xml_oarchive xml_out(outfile);
        xml_out << BOOST_SERIALIZATION_NVP(sch_master);
        outfile.close();

        std::ofstream symfile("inv_sym.xml", std::ios_base::out);
        boost::archive::xml_oarchive xml_out2(symfile);
        xml_out2 << BOOST_SERIALIZATION_NVP(sym_master);
        symfile.close();
    } catch (oa::oaCompatibilityError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA Compatibility Error: " + msg_std);
    } catch (oa::oaDMError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA DM Error: " + msg_std);
    } catch (oa::oaError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA Error: " + msg_std);
    } catch (oa::oaDesignError &ex) {
        throw std::runtime_error("OA Design Error: " + std::string(ex.getMsg()));
    }

    return 0;
}

int main(int argc, char *argv[]) {
    return read_oa();
}
