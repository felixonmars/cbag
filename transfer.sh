#!/usr/bin/env bash

rsync -av --delete include/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/include
rsync -av --delete src/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/src
rsync -av --delete test/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/test
rsync -v CMakeLists.txt erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/CMakeLists.txt
rsync -v build/install.sh erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/build/install.sh
