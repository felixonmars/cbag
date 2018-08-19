//
// Created by erichang on 7/17/18.
//

#ifndef CBAG_DATABASE_FIGURES_ADAPTED_H
#define CBAG_DATABASE_FIGURES_ADAPTED_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <cbag/database/figures.h>

BOOST_FUSION_ADAPT_STRUCT(cbag::Instance, lib_name, cell_name, view_name, xform, connections,
                          params)

BOOST_FUSION_ADAPT_STRUCT(cbag::SchPinObject, inst, attr)

BOOST_FUSION_ADAPT_STRUCT(cbag::PinFigure, obj, sig_type)

#endif // CBAG_DATABASE_FIGURES_ADAPTED_H
