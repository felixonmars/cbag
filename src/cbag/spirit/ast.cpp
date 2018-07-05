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

        }
    }
}