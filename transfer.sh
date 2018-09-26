#!/usr/bin/env bash

rsync -zv CMakeLists.txt erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/CMakeLists.txt
rsync -zv build.sh erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/build.sh
rsync -zarv --delete include/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/include
rsync -zarv --delete src/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/src
rsync -zarv --delete test/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/test
rsync -zarv --delete util/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/util
rsync -zv python/src_pybag/setup.py erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/python/src_pybag/setup.py
rsync -zarv --delete python/src_pybag/pybag/ --include="*/" --include="*.py" --include="*.pyx" \
--include="*.pxd" --exclude="*" erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/python/src_pybag/pybag
rsync -zv python/src_pybagoa/setup.py erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/python/src_pybagoa/setup.py
rsync -zarv --delete python/src_pybagoa/pybagoa/ --include="*/" --include="*.py" --include="*.pyx" \
--include="*.pxd" --exclude="*" erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/python/src_pybagoa/pybagoa
