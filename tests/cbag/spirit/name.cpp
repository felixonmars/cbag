#include <string>
#include <utility>

#include <catch2/catch.hpp>

#include <cbag/netlist/name_convert.h>
#include <cbag/spirit/ast.h>

SCENARIO("valid names", "[name_parse]") {
    std::pair<std::string, std::string> data =
        GENERATE(values<std::pair<std::string, std::string>>({
            {"foo", ""},
            {"bar0<3>", ""},
            {"bar_foo<0>", ""},
            {"bar_<3:5>", ""},
            {"ACG<5:1>", ""},
            {"ABC_a12<3:5:1>", "ABC_a12<3:5>"},
            {"bar<6:1:1>", "bar<6:1>"},
            {"<*1>baz", "baz"},
            {"<*1>(baz)", "baz"},
            {"<*3>baz", ""},
            {"<*6>baz<1>", ""},
            {"<*5>baz<2:6>", ""},
            {"<*5>(baz<2:6>)", "<*5>baz<2:6>"},
            {"<*4>baz<7:1>", ""},
            {"<*3>baz<2:6:1>", "<*3>baz<2:6>"},
            {"<*2>baz<7:1:2>", ""},
            {"foo,bar", ""},
            {"foo,bar<3>", ""},
            {"baz<3:1>,bar<1:5:1>", "baz<3:1>,bar<1:5>"},
            {"<*1>bar<3>,<*2>foo", "bar<3>,<*2>foo"},
            {"<*1>bar<3>,<*2>(foo)", "bar<3>,<*2>foo"},
            {"<*1>bar<3:10:2>,<*2>foo<1:3>", "bar<3:10:2>,<*2>foo<1:3>"},
            {"a,b,c", ""},
            {"<*2>(b,c)", ""},
            {"a,<*2>(b,c)", ""},
            {"<*2>(<*4>foo<3:5>,a)", ""},
            {"<*2>(<*4>foo<3:5>,a),<*2>(b,c)", ""},
        }));

    THEN("parsing works") {
        cbag::spirit::ast::name name_obj;
        try {
            name_obj = cbag::parse_cdba_name(data.first);
        } catch (std::invalid_argument &ex) {
            FAIL("failed to parse " << data.first << ", error: " << std::string(ex.what()));
        }
        THEN("name converts back to same string") {
            std::string expected = (data.second == "") ? data.first : data.second;
            REQUIRE(cbag::to_string_cdba(name_obj) == expected);
        }
    }
}

SCENARIO("invalid names", "[name_parse]") {
    std::string data = GENERATE(values<std::string>({
        "bar<1:2:0>",
        "<*0>foo_hi",
        "0baz",
        "haha,",
        "",
        "test<-1>",
        "tar<2:-3>",
        "taz<1:5:-1>",
        "<*3foo",
        "<*2>foo)",
        "<*2>(foo",
        "<*2>()",
    }));

    THEN("parsing fails") {
        CAPTURE(data);
        CHECK_THROWS_AS(cbag::parse_cdba_name(data), std::invalid_argument);
    }
}
