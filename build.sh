#!/usr/bin/env bash

cmake -H. -B_build -DCMAKE_BUILD_TYPE=Debug
cmake --build _build --target install -- -j 4

cd python/
${CBAG_PYTHON} setup.py build
cd ..
