#include <catch2/catch.hpp>

#include <cbagyaml/cbagyaml.h>

SCENARIO("parse cellview yaml", "[cbagyaml]") {
    GIVEN("A file") {
        std::string fname = "test/cbagyaml/data/sense_amp_strongarm.yaml";
        // TODO: re-enable test after we update the yaml file.
        /*
        THEN("cellview load properly") { auto cv = cbag::cv_from_file(fname); }
        */
    }
}
