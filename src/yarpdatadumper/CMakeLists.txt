# Copyright: (C) 2010 RobotCub Consortium
# Authors: Ugo Pattacini
# CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT


if(CREATE_YARPDATADUMPER)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)

  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS})

  if(YARP_HAS_OPENCV)
    add_definitions(-DADD_VIDEO)
    include_directories(${OpenCV_INCLUDE_DIRS})
  else()
    message(STATUS "yarpdatadumper: OpenCV not selected, keep on building...")
  endif()

  set(yarpdatadumper_SRCS main.cpp)

  source_group("Source Files" FILES ${yarpdatadumper_SRCS})


  add_executable(yarpdatadumper ${yarpdatadumper_SRCS})

  if(YARP_HAS_OPENCV)
    target_link_libraries(yarpdatadumper ${OpenCV_LIBRARIES})
  endif()

  target_link_libraries(yarpdatadumper YARP_OS
                                       YARP_init
                                       YARP_sig)

  install(TARGETS yarpdatadumper
          COMPONENT utilities
          DESTINATION "${CMAKE_INSTALL_BINDIR}")

  if(WIN32)
    # install the yarpdatadumperAppGenerator.py helper script
    yarp_install(PROGRAMS yarpdatadumperAppGenerator.py
                 COMPONENT utilities
                 DESTINATION "${CMAKE_INSTALL_BINDIR}")
  else()
    yarp_install(PROGRAMS yarpdatadumperAppGenerator.py
                 COMPONENT utilities
                 DESTINATION "${YARP_DATA_INSTALL_DIR}/scripts/")
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${YARP_DATA_INSTALL_DIR}/scripts/yarpdatadumperAppGenerator.py"
                                                               "${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator")
    install(CODE
 "message(STATUS \"Symbolic link: \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator\")
  if(NOT EXISTS \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/scripts/yarpdatadumperAppGenerator.py\")
    execute_process(COMMAND \${CMAKE_COMMAND} -E create_symlink \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${YARP_DATA_INSTALL_DIR}/scripts/yarpdatadumperAppGenerator.py\"
                                                               \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator\")
  endif()"
            COMPONENT utilities)

    # Remove deprecated files (since YARP 2.3.66.1)
    if(EXISTS "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator.py")
      message(STATUS "Deleted: \"${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator.py\"")
      file(REMOVE "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator.py")
    endif()
    if(EXISTS "${CMAKE_INSTALL_FULL_BINDIR}/yarpdatadumperAppGenerator.py")
      install(CODE
 "if(EXISTS \"\$ENV{DESTDIR}${CMAKE_INSTALL_FULL_BINDIR}/yarpdatadumperAppGenerator.py\")
    message(STATUS \"Deleted: \$ENV{DESTDIR}${CMAKE_INSTALL_FULL_BINDIR}/yarpdatadumperAppGenerator.py\")
    file(REMOVE \"\$ENV{DESTDIR}${CMAKE_INSTALL_FULL_BINDIR}/yarpdatadumperAppGenerator.py\")
  endif()")
    endif()

  endif()
endif()
