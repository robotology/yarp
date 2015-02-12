# Copyright: (C) 2011 RobotCub Consortium
# Authors: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

## add the "uninstall" target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conf/template/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               @ONLY)

add_custom_target(uninstall
                  COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
