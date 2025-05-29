# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpdatadumper)
  add_executable(yarpdatadumper)

  set(yarpdatadumper_SRCS
    main.cpp
  )

  target_sources(yarpdatadumper
    PRIVATE
      ${yarpdatadumper_SRCS})

  target_link_libraries(yarpdatadumper
    PRIVATE
      YARP::YARP_os
      YARP::YARP_init
      YARP::YARP_sig
  )

  if(YARP_HAS_OpenCV)
    target_compile_definitions(yarpdatadumper PRIVATE ADD_VIDEO)
    target_link_libraries(yarpdatadumper PRIVATE ${OpenCV_LIBS})
    target_link_libraries(yarpdatadumper PRIVATE YARP::YARP_cv)
  endif()

  install(
    TARGETS yarpdatadumper
    COMPONENT utilities
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )

  set_property(TARGET yarpdatadumper PROPERTY FOLDER "Command Line Tools")

endif()
