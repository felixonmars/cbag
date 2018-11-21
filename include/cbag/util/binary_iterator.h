#ifndef CBAG_UTIL_BINARY_ITERATOR_H
#define CBAG_UTIL_BINARY_ITERATOR_H

#include <optional>
#include <type_traits>

namespace cbag {
namespace util {

template <typename T> using IsInt = std::enable_if_t<std::is_integral_v<T>>;

template <typename T, typename = IsInt<T>> class binary_iterator {
  private:
    T low;
    std::optional<T> high;
    T current;
    std::optional<T> marker;

  public:
    binary_iterator(T low, T high) : low(low), high(high), current((high + low) / 2) {}

    binary_iterator(T low) : low(low), high(), current(low) {}

    bool has_next() const { return !high || low < *high; }

    T operator*() const { return current; }

    void up() {
        low = current + 1;
        if (high) {
            current = (low + *high) / 2;
        } else {
            if (current > 0) {
                current *= 2;
            } else {
                ++current;
            }
        }
    }

    void down() {
        high = current;
        current = (low + *high) / 2;
    }

    void save() { marker = current; }

    std::optional<T> get_save() const { return marker; }
}; // namespace util

} // namespace util
} // namespace cbag

#endif
