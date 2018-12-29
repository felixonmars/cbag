#include <catch2/catch.hpp>

#include <cbag/gdsii/main.h>
#include <cbag/layout/tech_util.h>

TEST_CASE("gds_lookup can read layer and object maps", "[gds]") {
    cbag::layout::tech obj("tests/data/test_layout/tech_params.yaml");
    cbag::gdsii::gds_lookup lookup(obj, "tests/data/test_gds/gds.layermap",
                                   "tests/data/test_gds/gds.objectmap");

    auto bnd_key = lookup.get_gds_layer(cbag::boundary_type::PR);
    REQUIRE(bool(bnd_key) == true);
    REQUIRE(*bnd_key == cbag::gdsii::gds_layer_t(1000, 0));

    auto lay_id = cbag::layout::layer_id_at(obj, "Poly");
    auto purp_id = cbag::layout::purpose_id_at(obj, "label");
    auto lay_key = lookup.get_gds_layer(cbag::layer_t(lay_id, purp_id));
    REQUIRE(bool(lay_key) == true);
    REQUIRE(*lay_key == cbag::gdsii::gds_layer_t(20, 46));
}
