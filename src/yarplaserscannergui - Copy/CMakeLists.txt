# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Author: Marco Randazzo marco.randazzo@iit.it
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

if(CREATE_YARPLASERSCANNERGUI)

  include(YarpOSXUtilities)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)

  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS}
					  ${OpenCV_INCLUDE_DIRS})

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)


  set(yarplaserscannergui_SRCS main.cpp)
 
  source_group("Source Files" FILES ${yarplaserscannergui_SRCS})
  source_group("Header Files" FILES ${yarplaserscannergui_HDRS})


 add_executable(yarplaserscannerguitest   ${yarplaserscannergui_SRCS}
                                        ${yarplaserscannergui_HDRS})

  target_link_libraries(yarplaserscannerguitest YARP_OS
                                        YARP_init
                                        YARP_sig
                                        YARP_dev)
	
  install(TARGETS yarplaserscannerguitest COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})


  yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarplaserscannerguitest
                                    TARGET_DEST yarplaserscannerguitest.app
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
