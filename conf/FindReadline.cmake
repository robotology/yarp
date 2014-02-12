# Try to find readline, a library for easy editing of command lines.
# Variables used by this module:
# READLINE_ROOT_DIR - Readline root directory
# Variables defined by this module:
# READLINE_FOUND - system has Readline
# READLINE_INCLUDE_DIR - the Readline include directory (cached)
# READLINE_INCLUDE_DIRS - the Readline include directories
# (identical to READLINE_INCLUDE_DIR)
# READLINE_LIBRARY - the Readline library (cached)

# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Elena Ceseracciu <elena.ceseracciu@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


find_path(READLINE_INCLUDE_DIR readline/readline.h
          HINTS ${READLINE_DIR} ENV READLINE_DIR PATH_SUFFIXES include)
find_library(READLINE_LIBRARY readline
          HINTS ${READLINE_DIR} ENV READLINE_DIR PATH_SUFFIXES lib)
mark_as_advanced(READLINE_INCLUDE_DIR READLINE_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Readline DEFAULT_MSG READLINE_LIBRARY READLINE_INCLUDE_DIR)

set(READLINE_INCLUDE_DIRS ${READLINE_INCLUDE_DIR})


