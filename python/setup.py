# -*- coding: utf-8 -*-

import os
from distutils.core import setup, Extension
from Cython.Build import cythonize

include_path = ['src']

include_dirs = [os.environ['OA_INCLUDE_DIR'],
                '../include',
                '../fmt/include',
]

libraries = ['cbag',
           'cbagyaml',
           'cbagoa',
           'oaCommon',
           'oaBase',
           'oaPlugIn',
           'oaDM',
           'oaDMFileSysBase',
           'oaTech',
           'oaDesign',
           'dl',
]

library_dirs = ['./lib',
              os.environ['OA_LINK_DIR'],
]
extra_compile_args = ["-std=c++17",
                      "-Wno-delete-non-virtual-dtor",
]
extra_link_args = ["-std=c++17",
                   "-Wl,--no-undefined",
]

setup(
    ext_modules=cythonize([
        Extension('pybag',
                  sources=[
                      'src/pybag.pyx',
                  ],
                  language='c++',
                  include_dirs=include_dirs,
                  libraries=libraries,
                  library_dirs=library_dirs,
                  extra_compile_args=extra_compile_args,
                  extra_link_args=extra_link_args,
        ),
        Extension('pybagoa',
                  sources=[
                      'src/pybagoa.pyx',
                  ],
                  language='c++',
                  include_dirs=include_dirs,
                  libraries=libraries,
                  library_dirs=library_dirs,
                  extra_compile_args=extra_compile_args,
                  extra_link_args=extra_link_args,
        ),
    ],
    include_path=include_path,
    ),
)
