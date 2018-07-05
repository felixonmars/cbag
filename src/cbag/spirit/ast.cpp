//
// Created by erichang on 7/5/18.
//

#include <cbag/spirit/ast.h>

namespace cbag {
    namespace spirit {
        namespace ast {
            inline uint32_t range::size() const {
                if (stop) {
                    uint32_t stop_val = *stop;
                    if (stop_val >= start) {
                        return (stop_val - start + step) / step;
                    } else {
                        return (start - stop_val + step) / step;
                    }
                } else {
                    return 1;
                }
            }

            inline uint32_t range::get_stop() const {
                return (stop) ? *stop : start;
            }

            inline uint32_t range::get_stop_exclude() const {
                if (stop) {
                    uint32_t stop_val = *stop;
                    if (stop_val >= start) {
                        return start + size() * step;
                    } else {
                        return start - size() * step;
                    }
                } else {
                    return start - step;
                }
            }

            inline YAML::Emitter &operator<<(YAML::Emitter &out, const range &v) {
                return out << YAML::Flow << YAML::BeginSeq << v.start << v.get_stop() << v.step
                           << YAML::EndSeq;
            }


        }
    }
}