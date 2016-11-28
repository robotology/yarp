#.rst:
# FindSQLite
# ----------
#
# Try to find the SQLite library.
# Once done this will define the following variables::
#
#  SQLite_FOUND         - System has SQLite
#  SQLite_INCLUDE_DIRS  - SQLite include directory
#  SQLite_LIBRARIES     - SQLite libraries
#  SQLite_DEFINITIONS   - Additional compiler flags for SQLite
#  SQLite_VERSION       - SQLite version
#  SQLite_MAJOR_VERSION - SQLite major version
#  SQLite_MINOR_VERSION - SQLite minor version
#  SQLite_PATCH_VERSION - SQLite patch version
#  SQLite_TWEAK_VERSION - SQLite tweak version

#=============================================================================
# Copyright 2012-2013  iCub Facility, Istituto Italiano di Tecnologia
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
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


include(StandardFindModule)
standard_find_module(SQLite sqlite3)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(SQLite PROPERTIES DESCRIPTION "Self-contained, serverless, zero-configuration, transactional SQL database engine"
                                             URL "http://sqlite.org/")
endif()
