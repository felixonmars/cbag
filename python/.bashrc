#!/bin/bash

export OA_ROOT=/home/erichang/programs/oa_dist
export OA_LINK_DIR="${OA_ROOT}/lib/linux_rhel50_gcc44x_64/opt"
export OA_INCLUDE_DIR="${OA_ROOT}/include"

export LD_LIBRARY_PATH=".:${OA_LINK_DIR}:${LD_LIBRARY_PATH:-}"
