#!/usr/bin/env bash

export BUILD_TYPE=${1:-Debug}
echo "CMAKE_BULD_TYPE: ${BUILD_TYPE}"
cmake -H. -B_build -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
cmake --build _build --target install -- -j 8

cd python/src_pybag
${CBAG_PYTHON} setup.py build
cd ../src_pybagoa
${CBAG_PYTHON} setup.py build
cd ../..
