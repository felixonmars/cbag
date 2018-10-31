#!/usr/bin/env bash

rsync -zv CMakeLists.txt erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/CMakeLists.txt
rsync -zv build_util.sh erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/build_util.sh
rsync -zv run_test.sh erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/run_test.sh
rsync -zarv --delete include/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/include
rsync -zarv --delete src/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/src
rsync -zarv --delete test/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/test
rsync -zarv --delete util/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/util
rsync -zarv --delete fmt/ erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/cbag/fmt
