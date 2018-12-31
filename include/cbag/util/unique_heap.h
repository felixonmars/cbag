#ifndef CBAG_UTIL_UNIQUE_HEAP_H
#define CBAG_UTIL_UNIQUE_HEAP_H

#include <queue>
#include <type_traits>
#include <unordered_set>

namespace cbag {
namespace util {

template <class T, class Hash = std::hash<T>> class unique_heap {
  private:
    template <class U> using IsT = std::enable_if_t<std::is_same_v<T, std::decay_t<U>>, int>;

    std::priority_queue<T> heap;
    std::unordered_set<T, Hash> items;

  public:
    unique_heap() = default;

    bool empty() const { return heap.empty(); }

    std::size_t size() const { return heap.size(); }

    template <class U, IsT<U> = 0> void push(U &&val) {
        auto result = items.insert(std::forward<U>(val));
        if (result.second) {
            heap.push(*(result.first));
        }
    }

    template <class... Args> void emplace(Args &&... args) {
        auto result = items.emplace(std::forward<Args>(args)...);
        if (result.second) {
            heap.push(*(result.first));
        }
    }

    const T &top() const { return heap.top(); }

    void pop() {
        items.erase(top());
        heap.pop();
    }
};

} // namespace util
} // namespace cbag

#endif
