# SPDX-FileCopyrightText: 2012-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

#[=======================================================================[.rst:
FindSOXR
-----------

Try to find the SOXR library.
Once done this will define the following variables::

 SOXR_FOUND         - System has SOXR
 SOXR_INCLUDE_DIRS  - SOXR include directory
 SOXR_LIBRARIES     - SOXR libraries
 SOXR_DEFINITIONS   - Additional compiler flags for SOXR
 SOXR_VERSION       - SOXR version
 SOXR_MAJOR_VERSION - SOXR major version
 SOXR_MINOR_VERSION - SOXR minor version
 SOXR_PATCH_VERSION - SOXR patch version
#]=======================================================================]

include(StandardFindModule)
standard_find_module(SOXR soxr
                     SKIP_CMAKE_CONFIG)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(SOXR PROPERTIES DESCRIPTION "The SoX Resampler library `libsoxr' performs one-dimensional sample-rate conversion"
                                              URL "https://sourceforge.net/projects/soxr/")
endif()
