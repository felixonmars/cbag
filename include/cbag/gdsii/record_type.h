
#ifndef CBAG_GDSII_RECORD_TYPE_H
#define CBAG_GDSII_RECORD_TYPE_H

#include <cstdint>

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
    ANGLE = 0x1C05,
    BOX = 0x2D00,
    BOXTYPE = 0x2E02,
};

} // namespace gdsii
} // namespace cbag

#endif
