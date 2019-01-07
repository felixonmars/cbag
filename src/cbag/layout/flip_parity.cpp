#include <boost/container_hash/hash.hpp>

#include <fmt/core.h>

#include <cbag/layout/flip_parity.h>

namespace cbag {
namespace layout {

flip_parity::flip_parity(std::vector<std::tuple<int, offset_t, offset_t>> &&data)
    : data_(std::move(data)) {}

bool flip_parity::operator==(const flip_parity &rhs) const noexcept { return data_ == rhs.data_; }

std::size_t flip_parity::get_hash() const noexcept {
    return boost::hash_range(data_.begin(), data_.end());
}

std::string flip_parity::to_string() const noexcept {
    auto n = data_.size();
    switch (n) {
    case 0:
        return "FlipParity[]";
    case 1:
        return fmt::format("FlipParity[({}, {}, {})]", std::get<0>(data_[0]), std::get<1>(data_[0]),
                           std::get<2>(data_[0]));
    default: {
        auto ans = fmt::format("FlipParity[({}, {}, {})", std::get<0>(data_[0]),
                               std::get<1>(data_[0]), std::get<2>(data_[0]));
        for (decltype(n) idx = 1; idx < n; ++idx) {
            ans += fmt::format(", ({}, {}, {})", std::get<0>(data_[idx]), std::get<1>(data_[idx]),
                               std::get<2>(data_[idx]));
        }
        ans += "]";
        return ans;
    }
    }
}

} // namespace layout
} // namespace cbag
