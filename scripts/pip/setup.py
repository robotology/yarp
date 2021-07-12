# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

import os
import sys
from pathlib import Path

import cmake_build_extension
from setuptools import setup

if (Path(".") / "CMakeLists.txt").exists():
    # Install from sdist
    source_dir = str(Path(".").absolute())
else:
    # Install from sources or build wheel
    source_dir = str(Path(".").absolute().parent.parent)

if "CIBUILDWHEEL" in os.environ and os.environ["CIBUILDWHEEL"] == "1":
    CIBW_CMAKE_OPTIONS = ["-DCMAKE_INSTALL_LIBDIR=lib"]
else:
    CIBW_CMAKE_OPTIONS = []

setup(
    cmdclass=dict(
        build_ext=cmake_build_extension.BuildExtension,
        sdist=cmake_build_extension.GitSdistFolder,
    ),
    ext_modules=[
        cmake_build_extension.CMakeExtension(
            name="CMakeProject",
            install_prefix="yarp",
            expose_binaries=[
                "bin/yarp",
                "bin/yarp-config",
                "bin/yarpdatadumper",
                "bin/yarpdev",
                "bin/yarphear",
                "bin/yarpidl_rosmsg",
                "bin/yarpidl_thrift",
                "bin/yarpmanager-console",
                "bin/yarprobotinterface",
                "bin/yarpros",
                "bin/yarprun",
                "bin/yarpserver",
            ],
            disable_editable=True,
            cmake_depends_on=["ycm_cmake_modules"],
            write_top_level_init="from .bindings.yarp import *",
            source_dir=source_dir,
            cmake_configure_options=[
                f"-DPython3_EXECUTABLE:PATH={sys.executable}",
                "-DCMAKE_BUILD_TYPE=Release",
                "-DBUILD_SHARED_LIBS:BOOL=OFF",
                "-DYARP_COMPILE_BINDINGS:BOOL=ON",
                "-DCREATE_PYTHON:BOOL=ON",
                "-DCMAKE_INSTALL_PYTHON3DIR:PATH='bindings/'",
                "-DYARP_COMPILE_EXAMPLES:BOOL=OFF",
                "-DYARP_COMPILE_GUIS:BOOL=OFF",
                "-DYARP_COMPILE_TESTS:BOOL=OFF",
                "-DYARP_COMPILE_UNMAINTAINED:BOOL=OFF",
            ]
            + CIBW_CMAKE_OPTIONS,
        )
    ],
)
