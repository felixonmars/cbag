//
// Created by erichang on 7/5/18.
//

#include <cbag/spirit/ast.h>

namespace cbag {
    namespace spirit {
        namespace ast {
            inline uint32_t range::size() const {
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

            inline YAML::Emitter &operator<<(YAML::Emitter &out, const range &v) {
                return out << YAML::Flow << YAML::BeginSeq << v.start << v.stop << v.step
                           << YAML::EndSeq;
            }


        }
    }
}