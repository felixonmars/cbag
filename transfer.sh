#!/usr/bin/env bash

rsync -av --delete include/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/include
rsync -av --delete src/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/src
rsync -av --delete test/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/test
rsync -av --delete python/src/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/python/src
rsync -v CMakeLists.txt erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/CMakeLists.txt
rsync -v install.sh erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/install.sh
rsync -v python/setup.py erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/python/setup.py
