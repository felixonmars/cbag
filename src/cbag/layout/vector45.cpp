#include <cstdlib>
#include <utility>

#include <cbag/layout/vector45.h>

namespace cbag {
namespace layout {

offset_t sign(offset_t x) { return (x > 0) - (x < 0); }

vector45 vector45::get_rotate90() const { return {dy, -dx}; }

vector45 vector45::get_norm() const { return {sign(dx), sign(dy)}; }

vector45 vector45::get_rotate90_norm() const { return {sign(dy), sign(-dx)}; }

bool vector45::valid() const { return dx == 0 || dy == 0 || abs(dx) == abs(dy); }

bool vector45::is_45_or_invalid() const { return dx != 0 && dy != 0; }

void vector45::invert() {
    dx = -dx;
    dy = -dy;
}

void vector45::rotate90() {
    offset_t tmp = dx;
    dx = dy;
    dy = -tmp;
}

void vector45::normalize() {
    dx = sign(dx);
    dy = sign(dy);
}

void vector45::rotate90_norm() {
    offset_t tmp = dx;
    dx = sign(dy);
    dy = sign(-tmp);
}

std::ostream &operator<<(std::ostream &os, const vector45 &d) {
    return os << '[' << d.dx << "; " << d.dy << ']';
}

} // namespace layout
} // namespace cbag
