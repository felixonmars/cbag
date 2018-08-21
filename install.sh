#!/usr/bin/env bash

cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target install -- -j 4
