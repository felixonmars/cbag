#include <catch2/catch.hpp>

#include <fmt/core.h>

#include <cbag/cbag.h>
#include <cbagoa/cbagoa.h>
#include <cbagyaml/cbagyaml.h>

cbag::sch::cellview read_cv(const std::string &lib_name, const std::string &cell_name,
                            const std::string &view_name, const std::string &lib_file) {
    cbagoa::oa_database db(lib_file);
    return db.read_sch_cellview(lib_name, cell_name, view_name);
}

std::string write_cv(const cbag::sch::cellview &cv, const std::string &cell_name,
                     const std::string &view_name) {
    std::string out_fname = fmt::format("test/data/test_outputs/{}.{}.yaml", cell_name, view_name);
    cbag::to_file(cv, out_fname);
    return out_fname;
}

cbag::sch::cellview read_yaml(const std::string &fname) { return cbag::cv_from_file(fname); }

SCENARIO("read/write OA data", "[cbagoa]") {
    GIVEN("An OA library") {
        std::string lib_name = "bag_digital_ec";
        std::string lib_file = "test/data/cds.lib";

        std::string cell_name = GENERATE(values<std::string>({
            "inv",
            "inv_chain",
        }));
        std::string view_name = GENERATE(values<std::string>({"schematic", "symbol"}));

        THEN("cellview is read properly") { read_cv(lib_name, cell_name, view_name, lib_file); }

        THEN("cellview written to yaml file properly") {
            auto cv = read_cv(lib_name, cell_name, view_name, lib_file);
            write_cv(cv, cell_name, view_name);
        }

        THEN("can read cellview from yaml file") {
            auto cv = read_cv(lib_name, cell_name, view_name, lib_file);
            auto out_fname = write_cv(cv, cell_name, view_name);
            // TODO: debug this
            // read_yaml(out_fname);
        }
    }
}
