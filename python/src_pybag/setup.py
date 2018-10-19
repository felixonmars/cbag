# -*- coding: utf-8 -*-

import os
import sysconfig
from setuptools import setup, Extension
from Cython.Build import cythonize

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
             # for some reason Berkeley cannot find python link library
             'python' + sysconfig.get_config_var('LDVERSION'),
             ]

library_dirs = ['../lib',
                os.environ['OA_LINK_DIR'],
                ]

extra_compile_args = ['-std=c++17',
                      '-Wno-delete-non-virtual-dtor',
                      ]

extra_link_args = ['-std=c++17',
                   '-Wl,--no-undefined',
                   ]

setup(
    install_requires=['cython'],
    packages=[pkg_name,
              pkg_name + '.util',
              pkg_name + '.layout',
              ],
    zip_safe=False,
    name=pkg_name,
    package_data={
        pkg_name: ['__init__.pxd',
                   'schematic.pxd',
                   'schematic.pyx',
                   os.path.join('util', '__init__.pxd'),
                   os.path.join('util', 'interval.pxd'),
                   os.path.join('util', 'interval.pyx'),
                   os.path.join('layout', '__init__.pxd'),
                   os.path.join('layout', 'util.pxd'),
                   os.path.join('layout', 'util.pyx'),
                   os.path.join('layout', 'cellview.pxd'),
                   os.path.join('layout', 'cellview.pyx'),
                   ]
    },
    ext_modules=cythonize(
        [
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
            Extension(pkg_name + '.util.interval',
                      sources=[
                          os.path.join(pkg_name, 'util', 'interval.pyx'),
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
            Extension(pkg_name + '.layout.cellview',
                      sources=[
                          os.path.join(pkg_name, 'layout', 'cellview.pyx'),
                      ],
                      language='c++',
                      include_dirs=include_dirs,
                      libraries=libraries,
                      library_dirs=library_dirs,
                      extra_compile_args=extra_compile_args,
                      extra_link_args=extra_link_args,
                      ),
        ],
        nthreads=8,
    ),
)
