#!/usr/bin/env bash

source ../../../.bashrc

export LD_LIBRARY_PATH="../../lib:${OA_LINK_DIR}"

${CBAG_PYTEST}
