#include <string>
#include <utility>

#include <catch2/catch.hpp>

#include <cbag/netlist/name_convert.h>
#include <cbag/spirit/ast.h>

SCENARIO("valid names", "[name]") {
    std::pair<std::string, std::string> data =
        GENERATE(values<std::pair<std::string, std::string>>({
            {"foo", ""},
            {"bar<3>", ""},
            {"bar<0>", ""},
            {"bar<3:5>", ""},
            {"bar<5:1>", ""},
            {"bar<3:5:1>", "bar<3:5>"},
            {"bar<6:1:1>", "bar<6:1>"},
            {"<*1>baz", "baz"},
            {"<*3>baz", ""},
            {"<*6>baz<1>", ""},
            {"<*5>baz<2:6>", ""},
            {"<*4>baz<7:1>", ""},
            {"<*3>baz<2:6:1>", "<*3>baz<2:6>"},
            {"<*2>baz<7:1:2>", ""},
            {"foo,bar", ""},
            {"foo,bar<3>", ""},
            {"baz<3:1>,bar<1:5:1>", "baz<3:1>,bar<1:5>"},
            {"<*1>bar<3>,<*2>foo", "bar<3>,<*2>foo"},
            {"<*1>bar<3:10:2>,<*2>foo<1:3>", "bar<3:10:2>,<*2>foo<1:3>"},
            {"a,b,c", ""},
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
