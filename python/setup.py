# -*- coding: utf-8 -*-

import os
from distutils.core import setup, Extension
from Cython.Build import cythonize

setup(
    ext_modules=cythonize(Extension('pybag',
                                    sources=[
                                        'src/pybag.pyx',
                                        # 'src/pybagoa.pyx',
                                    ],
                                    language='c++',
                                    include_dirs=[
                                        os.environ['OA_INCLUDE_DIR'],
                                        '../include',
                                        '../fmt/include',
                                    ],
                                    libraries=[
                                        'cbag',
                                        'cbagoa',
                                        'oaCommon',
                                        'oaBase',
                                        'oaPlugIn',
                                        'oaDM',
                                        'oaDMFileSysBase',
                                        'oaTech',
                                        'oaDesign',
                                        'dl',
                                    ],
                                    library_dirs=[
                                        './lib',
                                        os.environ['OA_LINK_DIR'],
                                    ],
                                    extra_compile_args=[
                                        "-std=c++17",
                                        "-Wno-unknown-pragmas",
                                        "-Wno-delete-non-virtual-dtor",
                                    ],
                                    extra_link_args=[
                                        "-std=c++17",
                                        "-Wl,--no-undefined",
                                    ],
    )
    ),
)
