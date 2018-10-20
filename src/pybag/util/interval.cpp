
#include <cbag/util/interval.h>
#include <pybag/util/interval.h>

namespace cu = cbag::util;

namespace pybag {
namespace util {

using py_intv_base_type = cu::interval;

class py_interval : public py_intv_base_type {
  public:
    using py_intv_base_type::py_intv_base_type;

    py::object value;

    py_interval(coord_type first, coord_type second, const py::object &obj)
        : py_intv_base_type(first, second), value(obj) {}
};
} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_util_interval(py::module &m_util) {
    py::module m = m_util.def_submodule("interval");

    m.doc() = "This module contains utility classes for manipulating intervals.";

    py::class_<cu::disjoint_intvs<pu::py_interval>>(m, "PyDisjointIntervals").def(py::init<>());
}
