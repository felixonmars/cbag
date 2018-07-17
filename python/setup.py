# -*- coding: utf-8 -*-

import os
from distutils.core import setup, Extension
from Cython.Build import cythonize

setup(
    ext_modules=cythonize(Extension('pybagoa',
                                    sources=[
                                        'src/pybagoa.pyx',
                                        '../src/cbag/spirit/ast.cpp',
                                        '../src/cbag/spirit/range.cpp',
                                        '../src/cbag/spirit/name_unit.cpp',
                                        '../src/cbag/spirit/name.cpp',
                                        '../src/cbagoa/name.cpp',
                                        '../src/cbagoa/read_oa.cpp',
                                        '../src/cbagoa/write_oa.cpp',
                                        '../src/cbagoa/database.cpp',
                                        '../easyloggingpp/src/easylogging++.cc',
                                    ],
                                    language='c++',
                                    include_dirs=[
                                        os.environ['OA_INCLUDE_DIR'],
                                        '../include',
                                    ],
                                    libraries=[
                                        'boost_system',
                                        'boost_serialization',
                                        'boost_filesystem',
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
                                        os.environ['OA_LINK_DIR'],
                                    ],
                                    extra_compile_args=[
                                        "-std=c++17",
                                        "-Wno-unknown-pragmas",
                                        "-Wno-delete-non-virtual-dtor",
                                    ],
                                    extra_link_args=[
                                        "-std=c++17",
                                    ],
                                    )
                          ),
)
