#include <catch2/catch.hpp>

#include <cbag/gdsii/write.h>
#include <cbag/layout/tech_util.h>

TEST_CASE("gds_lookup can read layer and object maps", "[gds]") {
    cbag::layout::tech obj("tests/data/test_layout/tech_params.yaml");
    cbag::gdsii::gds_lookup lookup(obj, "tests/data/test_gds/gds.layermap",
                                   "tests/data/test_gds/gds.objectmap");

    auto bnd_key = lookup.get_gds_layer(cbag::boundary_type::PR);
    REQUIRE(bool(bnd_key) == true);
    REQUIRE(*bnd_key == cbag::gdsii::gds_layer_t(1000, 0));

    auto lay_key = lookup.get_gds_layer(cbag::layout::layer_t_at(obj, "Poly", "label"));
    REQUIRE(bool(lay_key) == true);
    REQUIRE(*lay_key == cbag::gdsii::gds_layer_t(20, 46));
}
