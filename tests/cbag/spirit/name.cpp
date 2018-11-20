#include <string>
#include <tuple>
#include <utility>

#include <catch2/catch.hpp>

#include <cbag/spirit/util.h>
#include <cbag/util/name_convert.h>

class nb_iter_check {
  public:
    std::vector<std::string> *ptr;
    std::size_t nmax;

    nb_iter_check(std::vector<std::string> *vec, std::size_t n) : ptr(vec), nmax(n) {
        ptr->reserve(nmax);
    }

    nb_iter_check &operator*() { return *this; }
    nb_iter_check &operator=(std::string name) {
        if (ptr->size() >= nmax) {
            FAIL("Cannot append more than " + std::to_string(nmax) + " items.");
        }
        ptr->emplace_back(std::move(name));
        return *this;
    }
};

class par_iter_check {
  public:
    std::vector<std::string> *ptr;
    const cbag::spirit::namespace_info &ns;
    std::size_t nmax;

    par_iter_check(std::vector<std::string> *vec, const cbag::spirit::namespace_info &ns,
                   std::size_t n)
        : ptr(vec), ns(ns), nmax(n) {
        ptr->reserve(nmax);
    }

    par_iter_check &operator*() { return *this; }
    par_iter_check &operator=(const cbag::spirit::ast::name &name) {
        std::string cur_name = name.to_string(ns);
        if (ptr->size() >= nmax) {
            CAPTURE(*ptr);
            CAPTURE(cur_name);
            FAIL("Cannot append more than " + std::to_string(nmax) + " items.");
        }
        ptr->emplace_back(std::move(cur_name));
        return *this;
    }
};

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

    auto ns_info = cbag::spirit::get_ns_info(cbag::spirit::namespace_type::CDBA);

    THEN("parsing works") {
        cbag::spirit::ast::name name_obj;
        try {
            name_obj = cbag::util::parse_cdba_name(data.first);
        } catch (std::invalid_argument &ex) {
            FAIL("failed to parse " << data.first << ", error: " << std::string(ex.what()));
        }
        THEN("name converts back to same string") {
            std::string expected = (data.second == "") ? data.first : data.second;
            REQUIRE(name_obj.to_string(ns_info) == expected);
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
        CHECK_THROWS_AS(cbag::util::parse_cdba_name(data), std::invalid_argument);
    }
}

SCENARIO("get_name_bits_test", "[name_class]") {
    std::pair<std::string, std::vector<std::string>> data =
        GENERATE(values<std::pair<std::string, std::vector<std::string>>>({
            {"foo", {"foo"}},
            {"a,b", {"a", "b"}},
            {"foo<2>", {"foo<2>"}},
            {"foo<1:3>", {"foo<1>", "foo<2>", "foo<3>"}},
            {"foo<5:1:2>", {"foo<5>", "foo<3>", "foo<1>"}},
            {"foo<1:2>,a", {"foo<1>", "foo<2>", "a"}},
            {"<*3>a", {"a", "a", "a"}},
            {"<*2>a<2:1>", {"a<2>", "a<1>", "a<2>", "a<1>"}},
            {"<*2>(a,b)", {"a", "b", "a", "b"}},
            {"<*2>(a,b<1:0>)", {"a", "b<1>", "b<0>", "a", "b<1>", "b<0>"}},
            {"b,<*3>a", {"b", "a", "a", "a"}},
        }));

    auto ns_info = cbag::spirit::get_ns_info(cbag::spirit::namespace_type::CDBA);

    auto &test_name = data.first;
    auto &bit_list = data.second;

    CAPTURE(test_name);
    CAPTURE(bit_list);
    THEN("get_name_bits works") {
        cbag::spirit::ast::name name_obj;
        try {
            name_obj = cbag::util::parse_cdba_name(test_name);
        } catch (std::invalid_argument &ex) {
            FAIL("failed to parse " << test_name << ", error: " << std::string(ex.what()));
        }

        std::vector<std::string> output;
        std::size_t n = bit_list.size();
        nb_iter_check out_iter(&output, n);
        cbag::spirit::util::get_name_bits(name_obj, ns_info, out_iter);
        for (std::size_t idx = 0; idx < n; ++idx) {
            if (idx == output.size()) {
                FAIL("Output has " << idx << " items, but expected " << n);
            }
            std::string &expect = bit_list[idx];
            REQUIRE(output[idx] == expect);
        }
    }
}

SCENARIO("get_partition_test", "[name_class]") {
    std::tuple<std::string, std::vector<std::string>, uint32_t> data =
        GENERATE(values<std::tuple<std::string, std::vector<std::string>, uint32_t>>({
            // name_unit tests
            {"foo", {"foo"}, 1},
            {"foo<1>", {"foo<1>"}, 1},
            {"foo<2:0>", {"foo<2:0>"}, 3},
            {"foo<2:0>", {"foo<2>", "foo<1>", "foo<0>"}, 1},
            {"foo<5:0>", {"foo<5:4>", "foo<3:2>", "foo<1:0>"}, 2},
            {"foo<5:1>", {"foo<5:4>", "foo<3:2>", "foo<1>"}, 2},
            // name_rep tests
            {"<*3>foo", {"foo", "foo", "foo"}, 1},
            {"<*4>foo", {"<*2>foo", "<*2>foo"}, 2},
            {"<*3>foo<1>", {"<*2>foo<1>", "foo<1>"}, 2},
            {"<*2>foo<3:0>", {"foo<3:2>", "foo<1:0>", "foo<3:2>", "foo<1:0>"}, 2},
            {"<*3>foo<1:0>", {"foo<1:0>,foo<1>", "foo<0>,foo<1:0>"}, 3},
            {"<*3>(a,b)", {"a,b", "a,b", "a,b"}, 2},
            {"<*4>(a,b)", {"<*2>(a,b)", "<*2>(a,b)"}, 4},
            {"<*3>(a,b)", {"a,b,a", "b,a,b"}, 3},
            // name tests
            {"a,b,c", {"a", "b", "c"}, 1},
            {"a,<*2>b,c", {"a,b", "b,c"}, 2},
            {"a,<*4>b,c<0>", {"a,<*2>b", "<*2>b,c<0>"}, 3},
            {"a,foo<3:0>,c", {"a,foo<3:2>", "foo<1:0>,c"}, 3},
            {"a,foo<1:0>,bar<1:0>,c", {"a,foo<1:0>", "bar<1:0>,c"}, 3},
            {"a,foo<1:0>,bar<1:0>,c", {"a", "foo<1>", "foo<0>", "bar<1>", "bar<0>", "c"}, 1},
        }));

    auto ns_info = cbag::spirit::get_ns_info(cbag::spirit::namespace_type::CDBA);

    auto &test_name = std::get<0>(data);
    auto &str_list = std::get<1>(data);
    auto chunk = std::get<2>(data);

    CAPTURE(test_name);
    CAPTURE(str_list);
    CAPTURE(chunk);
    THEN("get_partition works") {
        cbag::spirit::ast::name name_obj;
        try {
            name_obj = cbag::util::parse_cdba_name(test_name);
        } catch (std::invalid_argument &ex) {
            FAIL("failed to parse " << test_name << ", error: " << std::string(ex.what()));
        }

        std::vector<std::string> output;
        std::size_t n = str_list.size();
        par_iter_check out_iter(&output, ns_info, n);
        cbag::spirit::util::get_partition(name_obj, chunk, out_iter);
        for (std::size_t idx = 0; idx < n; ++idx) {
            if (idx == output.size()) {
                FAIL("Output has " << idx << " items, but expected " << n);
            }
            std::string &expect = str_list[idx];
            REQUIRE(output[idx] == expect);
        }
    }
}

SCENARIO("repeat_test", "[name_parse]") {
    std::tuple<std::string, std::string, uint32_t> data =
        GENERATE(values<std::tuple<std::string, std::string, uint32_t>>({
            {"foo", "", 0},
            {"foo", "foo", 1},
            {"foo", "<*2>foo", 2},
            {"foo<1:3>", "<*2>foo<1:3>", 2},
            {"<*2>foo", "<*4>foo", 2},
            {"<*2>foo<0>", "<*6>foo<0>", 3},
            {"<*2>foo<1:2>", "<*6>foo<1:2>", 3},
            {"a,b", "<*3>(a,b)", 3},
            {"a,b<2:0>", "<*3>(a,b<2:0>)", 3},
            {"<*2>a,b", "<*3>(<*2>a,b)", 3},
        }));

    auto ns_info = cbag::spirit::get_ns_info(cbag::spirit::namespace_type::CDBA);

    std::string start = std::get<0>(data);
    std::string expect = std::get<1>(data);
    uint32_t mult = std::get<2>(data);

    CAPTURE(start);
    CAPTURE(expect);
    CAPTURE(mult);
    THEN("repeat works") {
        cbag::spirit::ast::name name_obj;
        try {
            name_obj = cbag::util::parse_cdba_name(start);
        } catch (std::invalid_argument &ex) {
            FAIL("failed to parse " << start << ", error: " << std::string(ex.what()));
        }
        std::string ans = name_obj.repeat(mult).to_string(ns_info);
        REQUIRE(ans == expect);
    }
}
