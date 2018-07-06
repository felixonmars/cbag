//
// Created by erichang on 7/5/18.
//

#include <cbag/spirit/ast.h>

namespace cbag {
    namespace spirit {
        namespace ast {
            inline uint32_t range::size() const {
                if (step == 0)
                    return 0;
                if (stop >= start) {
                    return (stop - start + step) / step;
                } else {
                    return (start - stop + step) / step;
                }
            }

            inline uint32_t range::get_stop_exclude() const {
                if (stop >= start) {
                    return start + size() * step;
                } else {
                    return start - size() * step;
                }
            }

            bool range::operator==(const range &other) const {
                return start == other.start && stop == other.stop && step == other.step;
            }

            bool range::operator<(const range &other) const {
                return start < other.start
                       || (start == other.start && (stop < other.stop
                                                    || (stop == other.stop && step < other.step)));
            }

            bool name_bit::operator==(const name_bit &other) const {
                return base == other.base && index == other.index;
            }

            bool name_bit::operator<(const name_bit &other) const {
                if (base < other.base) {
                    return true;
                } else if (base == other.base) {
                    if (index) {
                        return bool(other.index) && (*index < *other.index);
                    } else {
                        return bool(other.index);
                    }
                } else {
                    return false;
                }
            }

            bool name_unit::operator==(const name_unit &other) const {
                return base == other.base && index == other.index && mult == other.mult;
            }

            bool name_unit::operator<(const name_unit &other) const {
                return base < other.base
                       || (base == other.base && (index < other.index
                                                  || (index == other.index && mult < other.mult)));
            }

            bool name::operator<(const name &other) const {
                unsigned long size1 = unit_list.size();
                unsigned long size2 = other.unit_list.size();

                if (size1 < size2) {
                    return true;
                } else if (size1 == size2) {
                    for (unsigned long idx = 0; idx < size1; idx++) {
                        if (unit_list[idx] < other.unit_list[idx]) {
                            return true;
                        }
                    }
                    return false;
                } else {
                    return false;
                }
            }

        }
    }
}