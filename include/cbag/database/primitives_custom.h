/** \file primitives_custom.h
 *  \brief This file defines custom shape and geometry primitives.
 *
 *  In case user do not have access to OpenAccess Library, these custom
 * primitives will be used.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#ifndef CBAG_DATABASE_PRIMITIVES_CUSTOM_H
#define CBAG_DATABASE_PRIMITIVES_CUSTOM_H

#include <cstdint>
#include <vector>

namespace cbag {
typedef int32_t coord_t;
typedef uint32_t lay_t;
typedef uint32_t purp_t;
typedef uint32_t dist_t;

/** Orientation enum
 */
enum Orientation { R0, R90, R180, R270, MY, MYR90, MX, MXR90 };
constexpr Orientation oR0 = R0;
constexpr Orientation oR90 = R90;
constexpr Orientation oR180 = R180;
constexpr Orientation oR270 = R270;
constexpr Orientation oMY = MY;
constexpr Orientation oMYR90 = MYR90;
constexpr Orientation oMX = MX;
constexpr Orientation oMXR90 = MXR90;

/** Path style enum
 */
enum PathStyle {
    truncate,
    extend,
    round,
    variable,
};
constexpr PathStyle psTruncate = truncate;
constexpr PathStyle psExtend = extend;
constexpr PathStyle psRound = round;
constexpr PathStyle psVariable = variable;

/** Text alignment enum
 */
enum TextAlign {
    upperLeft,
    centerLeft,
    lowerLeft,
    upperCenter,
    centerCenter,
    lowerCenter,
    upperRight,
    centerRight,
    lowerRight,
};
constexpr TextAlign taUL = upperLeft;
constexpr TextAlign taCL = centerLeft;
constexpr TextAlign taLL = lowerLeft;
constexpr TextAlign taUC = upperCenter;
constexpr TextAlign taCC = centerCenter;
constexpr TextAlign taLC = lowerCenter;
constexpr TextAlign taUR = upperRight;
constexpr TextAlign taCR = centerRight;
constexpr TextAlign taLR = lowerRight;

/** Font enum
 */
enum Font {
    euroStyle,
    gothic,
    math,
    roman,
    script,
    stick,
    fixed,
    swedish,
    milSpec
};
constexpr Font fEuro = euroStyle;
constexpr Font fGoth = gothic;
constexpr Font fMath = math;
constexpr Font fRoman = roman;
constexpr Font fScript = script;
constexpr Font fStick = stick;
constexpr Font fFixed = fixed;
constexpr Font fSwedish = swedish;
constexpr Font fMilSpec = milSpec;

/** Terminal attribute type enum
 */
enum TermAttrType { name, hasPins, numBits };
constexpr TermAttrType tatName = TermAttrType::name;
constexpr TermAttrType tatHasPin = TermAttrType::hasPins;
constexpr TermAttrType tatNumBits = TermAttrType::numBits;

/** Text display format enum
 */
enum TextDispFormat { name, value, nameValue };
constexpr TextDispFormat tdfName = TextDispFormat::name;
constexpr TextDispFormat tdfValue = TextDispFormat::value;
constexpr TextDispFormat tdfNameValue = TextDispFormat::nameValue;

enum SigType {
    signal,
    power,
    ground,
    clock,
    tieOff,
    tieHi,
    tieLo,
    analog,
    scan,
    reset
};
constexpr SigType stSignal = SigType::signal;
constexpr SigType stPower = SigType::power;
constexpr SigType stGround = SigType::ground;
constexpr SigType stClock = SigType::clock;
constexpr SigType stTieoff = SigType::tieOff;
constexpr SigType stTieHi = SigType::tieHi;
constexpr SigType stTieLo = SigType::tieLo;
constexpr SigType stAnalog = SigType::analog;
constexpr SigType stScan = SigType::scan;
constexpr SigType stReset = SigType::reset;

enum TermType { input, output, inout };
constexpr TermType trmInput = TermType::input;
constexpr TermType trmOutput = TermType::output;
constexpr TermType trmInout = TermType::inout;

/** A point
 */
class Point {
  public:
    Point() : xv(0), yv(0) {}

    Point(coord_t xv, coord_t yv) : xv(xv), yv(yv) {}

    coord_t x() const { return xv; }

    coord_t y() const { return yv; }

    void set(coord_t x, coord_t y) {
        xv = x;
        yv = y;
    }

  private:
    coord_t xv, yv;
};

/** Represents locations/orientation of an instance.
 */
class Transform {
  public:
    Transform() : x(0), y(0), oval(oR0) {}

    Transform(coord_t x, coord_t y, Orientation orient = oR0)
        : x(x), y(y), oval(orient) {}

    coord_t xOffset() const { return x; }

    coord_t yOffset() const { return y; }

    Orientation orient() const { return oval; }

    void set(coord_t xv, coord_t yv, Orientation o) {
        x = xv;
        y = yv;
        oval = o;
    }

  private:
    coord_t x, y;
    Orientation oval;
};

/** A bounding box
 */
class BBox {
  public:
    BBox() : xl(0), yl(0), xh(0), yh(0) {}

    BBox(coord_t xl, coord_t yl, coord_t xh, coord_t yh)
        : xl(xl), yl(yl), xh(xh), yh(yh) {}

    coord_t left() const { return xl; }

    coord_t bottom() const { return yl; }

    coord_t right() const { return xh; }

    coord_t top() const { return yh; }

    void set(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
        xl = x0;
        yl = y0;
        xh = x1;
        yh = y1;
    }

  private:
    coord_t xl, yl, xh, yh;
};

class PointArray {
  public:
    explicit PointArray(uint32_t size = 0) : points(size) {}

    uint32_t getNumElements() const {
        return static_cast<uint32_t>(points.size());
    }

    const Point &operator[](uint32_t index) const { return points[index]; }

    Point &operator[](uint32_t index) { return points[index]; }

    void setSize(uint32_t new_size, int save_elements = 0) {
        points.resize(new_size);
    }

  private:
    std::vector<Point> points;
};

} // namespace cbag

#endif // CBAG_DATABASE_PRIMITIVES_CUSTOM_H
