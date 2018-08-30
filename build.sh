#!/usr/bin/env bash

cmake -H. -B_build -DCMAKE_BUILD_TYPE=Debug
cmake --build _build --target install -- -j 8

cd python/src_pybag
${CBAG_PYTHON} setup.py build
cd ../src_pybagoa
${CBAG_PYTHON} setup.py build
cd ../..
