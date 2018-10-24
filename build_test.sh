#!/usr/bin/env bash

source .bashrc

export BUILD_TYPE=${1:-Debug}
echo "CMAKE_BULD_TYPE: ${BUILD_TYPE}"
cmake -H. -B_build -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
cmake --build _build --target test_main -- -j 8
