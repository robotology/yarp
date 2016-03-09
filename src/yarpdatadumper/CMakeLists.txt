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
          DESTINATION ${CMAKE_INSTALL_BINDIR})

   # install the yarpdatadumperAppGenerator.py helper script
   yarp_install(PROGRAMS yarpdatadumperAppGenerator.py
                COMPONENT utilities
                DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
