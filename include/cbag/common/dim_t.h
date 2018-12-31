
#ifndef CBAG_COMMON_DIM_T_H
#define CBAG_COMMON_DIM_T_H

#include <array>
#include <tuple>

#include <cbag/common/typedefs.h>

namespace cbag {

class dim_t : public std::array<dist_t, 2> {
  public:
    template <std::size_t N> decltype(auto) get() const { return (*this)[N]; }
};

} // namespace cbag

namespace std {

template <> struct tuple_size<cbag::dim_t> : std::integral_constant<std::size_t, 2> {};

template <std::size_t N> struct tuple_element<N, cbag::dim_t> { using type = cbag::dist_t; };
} // namespace std

#endif
