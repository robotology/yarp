# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

macro(YARP_REMOVE_FILE _file)
  if(EXISTS "${CMAKE_BINARY_DIR}/${_file}")
    message(STATUS "Deleted: \"${CMAKE_BINARY_DIR}/${_file}\"")
    file(REMOVE "${CMAKE_BINARY_DIR}/${_file}")
  endif()
  install(CODE
 "if(EXISTS \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${_file}\")
    message(STATUS \"Deleted: \\\"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${_file}\\\"\")
    file(REMOVE \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${_file}\")
  endif()"
          COMPONENT scripts)
endmacro()

macro(YARP_REMOVE_DIRECTORY _dir)
  if(EXISTS "${CMAKE_BINARY_DIR}/${_dir}" AND
     IS_DIRECTORY "${CMAKE_BINARY_DIR}/${_dir}")
    message(STATUS "Deleted: \"${CMAKE_BINARY_DIR}/${_dir}\"")
    file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/${_dir}")
  endif()
  install(CODE
 "if(EXISTS \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${_dir}\" AND
     IS_DIRECTORY \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${_dir}\")
    message(STATUS \"Deleted: \\\"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${_dir}\\\"\")
    file(REMOVE_RECURSE \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${_dir}\")
  endif()"
          COMPONENT scripts)
endmacro()
