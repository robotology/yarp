# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

find_package(Perl QUIET)
if(PERL_FOUND AND NOT WIN32 AND IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.git")
  add_test(
    NAME misc::check_style
    COMMAND "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_style.pl"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  )

  add_test(
    NAME misc::check_license
    COMMAND "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_license.pl"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  )
endif()
