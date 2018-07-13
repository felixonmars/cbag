/** \file primitives.h
 *  \brief This file defines the shape and geometry primitives.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */


#ifndef CBAG_DATABASE_PRIMITIVES_H
#define CBAG_DATABASE_PRIMITIVES_H

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>

#if __has_include(<oa/oaDesignDB.h>)
// use OpenAccess primitive types
# include <cbag/database/primitives_oa.h>

#else
// use custom primitive types
# include <cbag/database/primitives_custom.h>
#endif

// serialization routines for the primitives
BOOST_SERIALIZATION_SPLIT_FREE(cbag::Transform)
BOOST_SERIALIZATION_SPLIT_FREE(cbag::Point)
BOOST_SERIALIZATION_SPLIT_FREE(cbag::BBox)
BOOST_SERIALIZATION_SPLIT_FREE(cbag::PointArray)

namespace boost {
    namespace serialization {


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

        // Transform

        template<class Archive>
        void save(Archive &ar, const cbag::Transform &v, const unsigned int version) {
            cbag::coord_t x = v.xOffset();
            cbag::coord_t y = v.yOffset();
            cbag::Orientation o = v.orient();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & make_nvp("x", x);
            ar & make_nvp("y", y);
            ar & make_nvp("orient", o);
#pragma clang diagnostic pop
        }

#pragma clang diagnostic pop


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

        template<class Archive>
        void load(Archive &ar, cbag::Transform &v, const unsigned int version) {
            cbag::coord_t x = 0;
            cbag::coord_t y = 0;
            cbag::Orientation orient = cbag::oR0;
            ar >> x;
            ar >> y;
            ar >> orient;
            v.set(x, y, orient);
        }

#pragma clang diagnostic pop

        // Point

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

        template<class Archive>
        void save(Archive &ar, const cbag::Point &v, const unsigned int version) {
            cbag::coord_t x = v.x();
            cbag::coord_t y = v.y();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & make_nvp("x", x);
            ar & make_nvp("y", y);
#pragma clang diagnostic pop
        }

#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

        template<class Archive>
        void load(Archive &ar, cbag::Point &v, const unsigned int version) {
            cbag::coord_t x = 0;
            cbag::coord_t y = 0;
            ar >> x;
            ar >> y;
            v.set(x, y);
        }

#pragma clang diagnostic pop

        // BBox

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

        template<class Archive>
        void save(Archive &ar, const cbag::BBox &v, const unsigned int version) {
            cbag::coord_t xl = v.left();
            cbag::coord_t yl = v.bottom();
            cbag::coord_t xh = v.right();
            cbag::coord_t yh = v.top();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & make_nvp("xl", xl);
            ar & make_nvp("yl", yl);
            ar & make_nvp("xh", xh);
            ar & make_nvp("yh", yh);
#pragma clang diagnostic pop
        }

#pragma clang diagnostic pop


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

        template<class Archive>
        void load(Archive &ar, cbag::BBox &v, const unsigned int version) {
            cbag::coord_t xl = 0;
            cbag::coord_t yl = 0;
            cbag::coord_t xh = 0;
            cbag::coord_t yh = 0;
            ar >> xl;
            ar >> yl;
            ar >> xh;
            ar >> yh;
            v.set(xl, yl, xh, yh);
        }

#pragma clang diagnostic pop

        // PointArray

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

        template<class Archive>
        void save(Archive &ar, const cbag::PointArray &v, const unsigned int version) {
            uint32_t n = v.getNumElements();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & make_nvp("n", n);
            for (uint32_t i = 0; i < n; ++i) {
                cbag::Point p = v[i];
                std::string key = "item" + std::to_string(i);
                ar & make_nvp(key.c_str(), p);
            }
#pragma clang diagnostic pop
        }

#pragma clang diagnostic pop


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

        template<class Archive>
        void load(Archive &ar, cbag::PointArray &v, const unsigned int version) {
            uint32_t n = 0;
            ar >> n;
            v.setSize(n);
            for (uint32_t i = 0; i < n; i++) {
                cbag::Point p;
                ar >> p;
                v[i] = p;
            }
        }

#pragma clang diagnostic pop

    }
}

#endif //CBAG_DATABASE_PRIMITIVES_H
