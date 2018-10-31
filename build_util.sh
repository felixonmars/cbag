#!/usr/bin/env bash

source .bashrc

export BUILD_TYPE=${1:-Debug}
echo "CMAKE_BULD_TYPE: ${BUILD_TYPE}"
cmake -H. -B_build -DCMAKE_BUILD_TYPE=${BUILD_TYPE} || exit 1
cmake --build _build --target write_cv_yaml -- -j 8
cmake --build _build --target write_tech_info -- -j 8
