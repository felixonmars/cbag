#!/usr/bin/env bash

if [ -z ${OA_LINK_DIR+x} ]
then
    echo "OA_LINK_DIR is unset"
    exit 1
fi

export BUILD_TYPE=${1:-Debug}
echo "CMAKE_BULD_TYPE: ${BUILD_TYPE}"
cmake -H. -B_build -DCMAKE_BUILD_TYPE=${BUILD_TYPE} || exit 1
cmake --build _build --target write_cv_yaml -- -j 8 || exit 1
cmake --build _build --target write_tech_info -- -j 8
