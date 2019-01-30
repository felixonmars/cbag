#include <sstream>

#include <catch2/catch.hpp>

#include <spdlog/spdlog.h>

#include <spdlog/sinks/stdout_color_sinks.h>

#include <cbag/common/transformation_util.h>
#include <cbag/gdsii/read_util.h>
#include <cbag/gdsii/write_util.h>

std::shared_ptr<spdlog::logger> get_catch_logger() {
    auto ans = spdlog::get("catch");
    if (!ans) {
        return spdlog::stdout_color_st("catch");
    }
    return ans;
}

TEST_CASE("Read/write 64-bit integers", "[gds]") {
    using data_type = uint64_t;

    auto val = GENERATE(values<data_type>({
        0x0,
        0x1234,
        0xdeadbeef,
        0x1000000000000000,
        0x4310e00000000000,
    }));

    std::stringstream stream;

    cbag::gdsii::write_bytes(stream, val);
    auto ans = cbag::gdsii::read_bytes<data_type>(stream);
    REQUIRE(ans == val);
}

TEST_CASE("Read/write transformation objects", "[gds]") {
    auto xform = GENERATE(values<cbag::transformation>({
        cbag::make_xform(0, 0, cbag::oR0),
        cbag::make_xform(0, 0, cbag::oR180),
        cbag::make_xform(0, 0, cbag::oMX),
        cbag::make_xform(5, 10, cbag::oR90),
        cbag::make_xform(-100, -100, cbag::oMX),
        cbag::make_xform(-5, -5, cbag::oMXR90),
        cbag::make_xform(-20, 15, cbag::oMYR90),
    }));

    auto logger = get_catch_logger();
    std::stringstream stream;

    cbag::gdsii::write_transform(*logger, stream, xform);
    auto ans = cbag::gdsii::read_transform(*logger, stream);
    REQUIRE(ans == xform);
}
