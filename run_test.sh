#!/usr/bin/env bash

export BUILD_TYPE=${1:-Debug}
echo "CMAKE_BULD_TYPE: ${BUILD_TYPE}"
cmake -H. -B_build -DCMAKE_BUILD_TYPE=${BUILD_TYPE} || exit 1
cmake --build _build --target test_main -- -j 8 || exit 1
./_build/tests/test_main
