#.rst:
# FindLibedit
# ------------
#
# Try to find NetBSD Editline library (libedit), a Berkeley-style licensed
# command line editor library provides generic line editing, history, and
# tokenization functions, similar to those found in GNU Readline.
# Once done this will define the following variables::
#
#  Libedit_FOUND         - System has Editline library
#  Libedit_INCLUDE_DIRS  - Editline library include directory
#  Libedit_LIBRARIES     - Editline library libraries
#  Libedit_DEFINITIONS   - Additional compiler flags for Editline library
#  Libedit_VERSION       - Editline library version
#  Libedit_MAJOR_VERSION - Editline library major version
#  Libedit_MINOR_VERSION - Editline library minor version
#
# Environment variables used to locate the Editline library::
#
#  READLINE_DIR - Libedit root directory
#
# Cached variables used to locate the NetBSD Editline library::
#
#  Libedit_INCLUDE_DIR - the Libedit include directory
#  Libedit_LIBRARY_RELEASE - NetBSD Editline library (release)
#  Libedit_LIBRARY_DEBUG - NetBSD Editline library  (debug)

#=============================================================================
# Copyright 2016 iCub Facility, Istituto Italiano di Tecnologia
#   Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
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
standard_find_module(Libedit libedit
                     TARGET Libedit::libedit)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(Libedit PROPERTIES DESCRIPTION "A command line editor library provides generic line editing, history, and tokenization functions"
                                              URL "http://thrysoee.dk/editline/")
endif()
