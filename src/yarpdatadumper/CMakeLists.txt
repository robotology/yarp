# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(CREATE_YARPDATADUMPER)
  if(YARP_HAS_OPENCV)
    add_definitions(-DADD_VIDEO)
    include_directories(${OpenCV_INCLUDE_DIRS})
  else()
    message(STATUS "yarpdatadumper: OpenCV not selected, keep on building...")
  endif()

  set(yarpdatadumper_SRCS main.cpp)

  add_executable(yarpdatadumper ${yarpdatadumper_SRCS})

  if(YARP_HAS_OPENCV)
    target_link_libraries(yarpdatadumper ${OpenCV_LIBRARIES})
  endif()

  target_link_libraries(yarpdatadumper YARP::YARP_OS
                                       YARP::YARP_init
                                       YARP::YARP_sig)

  install(TARGETS yarpdatadumper
          COMPONENT utilities
          DESTINATION "${CMAKE_INSTALL_BINDIR}")

  set_property(TARGET yarpdatadumper PROPERTY FOLDER "Command Line Tools")

  if(CMAKE_HOST_WIN32)
    # install the yarpdatadumperAppGenerator.py helper script
    yarp_install(PROGRAMS yarpdatadumperAppGenerator.py
                 COMPONENT utilities
                 DESTINATION "${CMAKE_INSTALL_BINDIR}")
  else()
    yarp_install(PROGRAMS yarpdatadumperAppGenerator.py
                 COMPONENT utilities
                 DESTINATION "${YARP_DATA_INSTALL_DIR}/scripts/")
    file(RELATIVE_PATH _relpath "/${CMAKE_INSTALL_BINDIR}" "/${YARP_DATA_INSTALL_DIR}/scripts/yarpdatadumperAppGenerator.py")
    if (NOT EXISTS "${CMAKE_INSTALL_BINDIR}")
      execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_BINDIR}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${_relpath}" "${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator")
    install(CODE
 "message(STATUS \"Symbolic link: \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator to ${_relpath}\")
  if (NOT EXISTS \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/\")
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/\")
  endif()
  execute_process(COMMAND \${CMAKE_COMMAND} -E create_symlink \"${_relpath}\"
                                                               \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator\")"
            COMPONENT utilities)

    # Remove deprecated files (since YARP 2.3.66.1)
    if(EXISTS "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator.py")
      message(STATUS "Deleted: \"${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator.py\"")
      file(REMOVE "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator.py")
    endif()
    install(CODE
 "if(EXISTS \"\$ENV{DESTDIR}${CMAKE_INSTALL_FULL_BINDIR}/yarpdatadumperAppGenerator.py\")
    message(STATUS \"Deleted: \$ENV{DESTDIR}${CMAKE_INSTALL_FULL_BINDIR}/yarpdatadumperAppGenerator.py\")
    file(REMOVE \"\$ENV{DESTDIR}${CMAKE_INSTALL_FULL_BINDIR}/yarpdatadumperAppGenerator.py\")
  endif()")

  endif()
endif()
