# -*- coding: utf-8 -*-

import os
from setuptools import setup, Extension
from Cython.Distutils import build_ext


pkg_name = 'pybag'

include_dirs = [os.environ['OA_INCLUDE_DIR'],
                '../../include',
                '../../fmt/include',
]

libraries = ['cbag',
             'cbagyaml',
             'oaCommon',
             'oaBase',
             'oaPlugIn',
             'oaDM',
             'oaDMFileSysBase',
             'oaTech',
             'oaDesign',
             'dl',
]

library_dirs = ['../lib',
              os.environ['OA_LINK_DIR'],
]

extra_compile_args = ["-std=c++17",
                      "-Wno-delete-non-virtual-dtor",
]

extra_link_args = ["-std=c++17",
                   "-Wl,--no-undefined",
]

setup(
    install_requires=['cython'],
    packages=[pkg_name,
              pkg_name + '.layout',
    ],
    zip_safe=False,
    name=pkg_name,
    cmdclass={'build_ext': build_ext},
    package_data={
        pkg_name: ['__init__.pxd',
                   'schematic.pxd',
                   'schematic.pyx',
                   os.path.join('layout', '__init__.pxd'),
                   os.path.join('layout', 'util.pxd'),
                   os.path.join('layout', 'util.pyx'),
        ]
    },
    ext_modules=[
        Extension(pkg_name + '.schematic',
                  sources=[
                      os.path.join(pkg_name, 'schematic.pyx'),
                  ],
                  language='c++',
                  include_dirs=include_dirs,
                  libraries=libraries,
                  library_dirs=library_dirs,
                  extra_compile_args=extra_compile_args,
                  extra_link_args=extra_link_args,
        ),
        Extension(pkg_name + '.layout.util',
                  sources=[
                      os.path.join(pkg_name, 'layout', 'util.pyx'),
                  ],
                  language='c++',
                  include_dirs=include_dirs,
                  libraries=libraries,
                  library_dirs=library_dirs,
                  extra_compile_args=extra_compile_args,
                  extra_link_args=extra_link_args,
        ),
    ],
)
