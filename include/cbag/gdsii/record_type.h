
#ifndef CBAG_GDSII_RECORD_TYPE_H
#define CBAG_GDSII_RECORD_TYPE_H

#include <cstdint>
#include <stdexcept>
#include <string>

#include <fmt/core.h>

namespace cbag {
namespace gdsii {

enum class record_type : uint16_t {
    HEADER = 0x0002,
    BGNLIB = 0x0102,
    LIBNAME = 0x0206,
    UNITS = 0x0305,
    ENDLIB = 0x0400,
    BGNSTR = 0x0502,
    STRNAME = 0x0606,
    ENDSTR = 0x0700,
    BOUNDARY = 0x0800,
    SREF = 0x0A00,
    AREF = 0x0B00,
    TEXT = 0x0C00,
    LAYER = 0x0D02,
    DATATYPE = 0x0E02,
    WIDTH = 0x0F03,
    XY = 0x1003,
    ENDEL = 0x1100,
    SNAME = 0x1206,
    COLROW = 0x1302,
    TEXTTYPE = 0x1602,
    PRESENTATION = 0x1701,
    STRING = 0x1906,
    STRANS = 0x1A01,
    MAG = 0x1B05,
    ANGLE = 0x1C05,
    PROPATTR = 0x2B02,
    PROPVALUE = 0x2C06,
    BOX = 0x2D00,
    BOXTYPE = 0x2E02,
};

constexpr auto PROP_INST_NAME = 1;

inline std::string to_string(record_type rec) {
    switch (rec) {
    case record_type::HEADER:
        return "HEADER";
    case record_type::BGNLIB:
        return "BGNLIB";
    case record_type::LIBNAME:
        return "LIBNAME";
    case record_type::UNITS:
        return "UNITS";
    case record_type::ENDLIB:
        return "ENDLIB";
    case record_type::BGNSTR:
        return "BGNSTR";
    case record_type::STRNAME:
        return "STRNAME";
    case record_type::ENDSTR:
        return "ENDSTR";
    case record_type::BOUNDARY:
        return "BOUNDARY";
    case record_type::SREF:
        return "SREF";
    case record_type::AREF:
        return "AREF";
    case record_type::TEXT:
        return "TEXT";
    case record_type::LAYER:
        return "LAYER";
    case record_type::DATATYPE:
        return "DATATYPE";
    case record_type::WIDTH:
        return "WIDTH";
    case record_type::XY:
        return "XY";
    case record_type::ENDEL:
        return "ENDEL";
    case record_type::SNAME:
        return "SNAME";
    case record_type::COLROW:
        return "COLROW";
    case record_type::TEXTTYPE:
        return "TEXTTYPE";
    case record_type::PRESENTATION:
        return "PRESENTATION";
    case record_type::STRING:
        return "STRING";
    case record_type::STRANS:
        return "STRANS";
    case record_type::MAG:
        return "MAG";
    case record_type::ANGLE:
        return "ANGLE";
    case record_type::PROPATTR:
        return "PROPATTR";
    case record_type::PROPVALUE:
        return "PROPVALUE";
    case record_type::BOX:
        return "BOX";
    case record_type::BOXTYPE:
        return "BOXTYPE";
    default:
        throw std::invalid_argument(
            fmt::format("Unknown record type: {:#06x}", static_cast<uint16_t>(rec)));
    }
}

} // namespace gdsii
} // namespace cbag

#endif
