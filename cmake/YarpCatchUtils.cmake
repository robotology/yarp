# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

#########################################################################
# Wrapper for the catch_discover_tests that also enables colors, and sets
# the TIMEOUT and SKIP_RETURN_CODE test properties.

include (Catch)

function(yarp_catch_discover_tests _target)
  # Workaround to force catch_discover_tests to run tests under valgrind
  set_property(TARGET ${_target} PROPERTY CROSSCOMPILING_EMULATOR "${YARP_TEST_LAUNCHER}")
  catch_discover_tests(
    ${_target}
    EXTRA_ARGS "-s" "--colour-mode default"
    PROPERTIES
      TIMEOUT ${YARP_TEST_TIMEOUT}
      SKIP_RETURN_CODE 254
    )
endfunction()
