# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

find_package(Perl QUIET)
if(PERL_FOUND AND NOT WIN32 AND IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.git")
  add_test(NAME misc::check_style
           COMMAND "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_style.pl"
           WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

  add_test(NAME misc::check_license
           COMMAND "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_license.pl"
           WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()
