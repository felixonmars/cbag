#include <catch2/catch.hpp>

#include <filesystem>

#include <cbag/util/io.h>

namespace fs = std::filesystem;

TEST_CASE("can open file for read when there's no write permission", "[io]") {
    constexpr auto default_perms = fs::perms::owner_read | fs::perms::owner_write |
                                   fs::perms::group_read | fs::perms::others_read;
    auto fname = std::string("tests/data/test_io/io.txt");

    // set permission to no write
    auto fpath = fs::path(fname);
    fs::permissions(fpath, fs::perms::owner_read);
    auto stream = cbag::util::open_file_read(fname);
    auto result = stream.fail();
    stream.close();
    fs::permissions(fpath, default_perms);

    REQUIRE(result == false);
}

TEST_CASE("throw exception when reading file with no read permission", "[io]") {
    constexpr auto default_perms = fs::perms::owner_read | fs::perms::owner_write |
                                   fs::perms::group_read | fs::perms::others_read;
    auto fname = std::string("tests/data/test_io/io.txt");

    // set permission to no write
    auto fpath = fs::path(fname);
    fs::permissions(fpath, fs::perms::owner_write);
    auto throws_exception = false;
    try {
        auto stream = cbag::util::open_file_read(fname);
        stream.close();
    } catch (...) {
        throws_exception = true;
    }
    fs::permissions(fpath, default_perms);

    REQUIRE(throws_exception == true);
}

TEST_CASE("throw exception when writing file with no write permission", "[io]") {
    constexpr auto default_perms = fs::perms::owner_read | fs::perms::owner_write |
                                   fs::perms::group_read | fs::perms::others_read;
    auto fname = std::string("tests/data/test_io/io.txt");

    // set permission to no write
    auto fpath = fs::path(fname);
    fs::permissions(fpath, fs::perms::owner_read);
    auto throws_exception = false;
    try {
        auto stream = cbag::util::open_file_write(fname);
        stream.close();
    } catch (...) {
        throws_exception = true;
    }
    fs::permissions(fpath, default_perms);

    REQUIRE(throws_exception == true);
}
