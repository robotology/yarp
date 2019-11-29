#.rst:
# FindAtlas
# ---------
#
# Find the Atlas (and Lapack) libraries

#=============================================================================
# Copyright 2010 RobotCub Consortium
#   Authors: Arjan Gijsberts <arjan.gijsberts@idiap.ch>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)


set(ATLAS_POSSIBLE_INCLUDE_PATHS
    /usr/include
    /usr/include/atlas
    /usr/local/include
    $ENV{ATLAS_DIR}
    $ENV{ATLAS_DIR}/include
)

# Ubuntu's package management does not handle blas elegantly, causing
# many possible locations here.
set(ATLAS_POSSIBLE_LIBRARY_PATHS
    /usr/lib/libatlas-corei7sse3
    /usr/lib/atlas-amd64sse3
    /usr/lib/atlas-base
    /usr/lib/sse2
    /usr/lib/sse
    /usr/local/lib/sse2
    /usr/local/lib/sse
    /usr/lib
    /usr/local/lib
    $ENV{ATLAS_DIR}
    $ENV{ATLAS_DIR}/lib
)

find_path(ATLAS_CBLAS_INCLUDE_DIR NAMES cblas.h PATHS ${ATLAS_POSSIBLE_INCLUDE_PATHS})
find_path(ATLAS_CLAPACK_INCLUDE_DIR NAMES clapack.h PATHS ${ATLAS_POSSIBLE_INCLUDE_PATHS})
find_library(ATLAS_CBLAS_LIBRARY NAMES ptcblas_r ptcblas cblas_r cblas PATHS ${ATLAS_POSSIBLE_LIBRARY_PATHS})
find_library(ATLAS_ATLAS_LIBRARY NAMES atlas_r atlas PATHS ${ATLAS_POSSIBLE_LIBRARY_PATHS})
find_library(ATLAS_LAPACK_ATLAS_LIBRARY NAMES alapack_r alapack lapack_atlas PATHS ${ATLAS_POSSIBLE_LIBRARY_PATHS})

set(ATLAS_FOUND ON)


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Atlas
                                  DEFAULT_MSG
                                  ATLAS_ATLAS_LIBRARY
                                  ATLAS_CBLAS_INCLUDE_DIR
                                  ATLAS_CLAPACK_INCLUDE_DIR
                                  ATLAS_LAPACK_ATLAS_LIBRARY
                                  ATLAS_CBLAS_LIBRARY
)

mark_as_advanced(ATLAS_INCLUDE_DIR
                 ATLAS_CBLAS_INCLUDE_DIR
                 ATLAS_CLAPACK_INCLUDE_DIR
                 ATLAS_LIBRARIES
                 ATLAS_CBLAS_LIBRARY
                 ATLAS_ATLAS_LIBRARY
                 ATLAS_LAPACK_ATLAS_LIBRARY)

set(Atlas_FOUND ${ATLAS_FOUND})

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(Atlas PROPERTIES DESCRIPTION "Automatically Tuned Linear Algebra Software"
                                            URL "http://math-atlas.sourceforge.net/")
endif(COMMAND set_package_properties)
