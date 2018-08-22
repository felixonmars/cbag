#!/usr/bin/env bash

rsync -v CMakeLists.txt erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/CMakeLists.txt
rsync -v build.sh erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/build.sh
rsync -av --delete include/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/include
rsync -av --delete src/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/src
rsync -av --delete test/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/test
rsync -v python/setup.py erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/python/setup.py
rsync -v python/src/pybag.pyx erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/python/src/pybag.pyx
