# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# The following tests are disabled because they are already individually executed by
# Github CI, there is no need to perform them twice.
if (0)
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

    find_package(Python QUIET)
    if(PYTHON_FOUND AND NOT WIN32 AND IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.git")
      add_test(
        NAME misc::check_tests
        COMMAND "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_tests.py"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      )
    endif()
endif()
